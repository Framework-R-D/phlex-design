#include "calculate.h"

#include <algorithm>
#include <flat_set>
#include <list>
#include <ranges>
#include <vector>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

// Using declarations to make ranges easier
namespace ranges = std::ranges;
namespace views = std::ranges::views;

// Typedefs to refer to concrete lists of product or node
// Use lists for pointer stability (re-evaluate later for efficiency)
template <typename T> using list_t = std::list<T>;
using product_list_t = list_t<product>;
using node_list_t = list_t<node>;
using node_spec_list_t = list_t<node_spec>;

void make_initial_products(std::vector<init_prod> const& initial_products,
                           product_list_t& product_list, node_list_t& node_list,
                           node_spec_list_t& provider_spec_list) {
    fmt::print("Making initial products\n");
    assert(product_list.empty() && node_list.empty() && provider_spec_list.empty()
           && "Non-empty list before start");
    assert(ranges::all_of(initial_products, &init_prod::validate)
           && "Initial product didn't validate");

    // Populate with initial products
    for (const auto& prod : initial_products) {
        // First validate the layer path
        if (prod.layer.empty() or prod.layer.front() != "job"_idq) {
            throw std::logic_error(
                  fmt::format("Initial product {} has an invalid layer path!", prod.name));
        }

        // Make the provider first, then the product, then fill in the outputs list
        node_spec const& provider_spec = provider_spec_list.emplace_back(
              node_spec{.type = node_type::provider,
                        .name = prod.creator,
                        .target_layer_name = prod.layer.back(),
                        .output_names = std::vector{prod.name}});

        node& provider = node_list.emplace_back(&provider_spec, std::vector<product const*>{},
                                                std::vector<product const*>{}, prod.layer);
        product& new_prod = product_list.emplace_back(
              product{.name = prod.name, .current_job = false, .creator = &provider});

        provider.outputs.push_back(&new_prod);
    }
}

void make_nodes(std::vector<node_spec> const& algorithms, node_list_t& node_list,
                product_list_t& product_list) {
    fmt::print("Making nodes\n");
    assert(ranges::all_of(algorithms, &node_spec::validate)
           && "Algorithm specification didn't validate");
    assert(ranges::all_of(algorithms,
                          [](node_spec const& s) { return s.type != node_type::provider; })
           && "This prototype doesn't support explicit providers");
    // Fully populate the node list, though these won't be filled in yet
    for (const auto& spec : algorithms) {
        // Make sure the input queries are valid
        assert(ranges::all_of(spec.input_queries, &input_query::validate) && "Invalid input query");
        // Make sure outputs are distinguished
        assert((spec.output_names.size() == std::flat_set(spec.output_names).size())
               && "Duplicate output names for one creator");
        // Prefill the input product slots
        node& new_node = node_list.emplace_back(
              &spec, std::vector(spec.input_queries.size(), static_cast<product const*>(nullptr)));
        // Check we have a target layer name if this is a fold or unfold
        if ((spec.type == node_type::fold or spec.type == node_type::unfold)
            and !spec.target_layer_name.has_value()) {
            throw std::logic_error(fmt::format("The {} requires a target layer name!", new_node));
        }
        // Make the output products
        // These products won't have layers yet
        for (auto const& out_name : spec.output_names) {
            product& output = product_list.emplace_back(
                  product{.name = out_name, .current_job = true, .creator = &new_node});
            new_node.outputs.push_back(&output);
        }
        assert(new_node.outputs_filled());
    }
}

using candidate_set = std::vector<product const*>;
// bool duplicate_and_connect_inputs(node& node, product_list_t const& products,

bool connect_inputs(node& node, product_list_t const& products) {
    // Reconnect all inputs unconditionally
    int slot = 0;
    int const n_slots = node.spec->input_queries.size();
    bool modified = false;
    for (auto&& [query, link] : views::zip(node.spec->input_queries, node.inputs)) {
        ++slot;
        auto match = [&query = std::as_const(query)](product const& p) {
            // name is mandatory
            if (p.name != query.name) {
                return false;
            }
            if (query.creator_name && (p.creator->spec->name != query.creator_name)) {
                return false;
            }
            if (query.layer_name && (!p.filled_in() || p.layer().back() != query.layer_name)) {
                // If the query has a layer_name it will only match after the corresponding product
                // has had it's layer name determined
                return false;
            }
            if ((query.stage != stage_t::any) && (p.stage() != query.stage)) {
                return false;
            }
            return true;
        };
        auto matching_products = views::filter(products, match)
                                 | views::transform(
                                       [](product const& p) { return std::addressof(p); })
                                 | ranges::to<candidate_set>();
        // No matching products ==> skip on this round
        if (matching_products.empty()) {
            continue;
        }
        // One match ==> connect it
        if (matching_products.size() == 1) {
            if (link != matching_products[0]) {
                link = matching_products[0];
                fmt::print("Connected slot {}/{} for {} to {}\n", slot, n_slots, node, *link);
                modified = true;
            }
            continue;
        }
        // Otherwise, things get complicated.
        // This function only needs to handle the case where we have multiple matches because we
        // have a product created in this job and products created in earlier jobs. The case of
        // having the same layer name in multiple paths is handled duplicate_and_connect_nodes.
        int const distinct_stages = (views::transform(matching_products,
                                                      [](product const* p) { return p->stage(); })
                                     | ranges::to<std::flat_set>())
                                          .size();
        if (distinct_stages <= 1) {
            // This is an error for now
            fmt::print(
                  "ERR: Multiple matching products for query\n - {}\n",
                  fmt::join(matching_products | views::transform([](auto const* p) -> auto const& {
                                return *p;
                            }),
                            "\n - "));
            throw std::runtime_error(fmt::format("Error connecting inputs for {}. Have {} products "
                                                 "in candidate set and only {} distinct stages.",
                                                 node, matching_products.size(), distinct_stages));
        }
        auto matching_current_products = matching_products | views::filter([](product const* p) {
                                             return p->current_job;
                                         });
        if (int n_match = ranges::distance(matching_current_products); n_match != 1) {
            throw std::runtime_error(
                  fmt::format("Error connecting inputs for {}. Have {} products in candidate set.",
                              node, n_match));
        }
        if (link != *ranges::cbegin(matching_current_products)) {
            link = *ranges::cbegin(matching_current_products);
            modified = true;
            fmt::print("Connected slot {}/{} for {} to {}\n", slot, n_slots, node, *link);
        }
    }
    return modified;
}

bool connect_inputs(node_list_t& nodes, product_list_t const& products) {
    // Connect any inputs that we can now resolve
    bool modified = false;
    for (node& node : nodes) {
      // Only consider nodes that don't need to be duplicated.
      // Specifically, if any of a nodes input queries explicitly state a layer name AND 
        modified |= connect_inputs(node, products);
    }
    if (!modified) {
        fmt::print("No changes\n");
    }
    return modified;
}

bool update_layers(node_list_t& nodes, product_list_t const& products) {
    bool modified = false;
    for (node& node : nodes) {
        if (!node.inputs.empty() && node.inputs_connected()) {
            auto input_layers = node.inputs | views::transform([](product const* p) {
                                    return std::cref(p->layer());
                                })
                                | ranges::to<layer_path_list>();
            bool const hierarchy_ok = forms_hierarchy(input_layers);
            if (!hierarchy_ok) {
                auto input_layer_names = input_layers | views::transform([](auto const& lp) {
                                             return fmt_lp(lp);
                                         })
                                         | ranges::to<std::vector<std::string>>();
                throw std::runtime_error(fmt::format(
                      "Reconnected inputs for {} but input layers do not form a hierarchy. "
                      "Input layers are:\n  - {}\n",
                      node, fmt::join(input_layer_names, "\n  - ")));
            }
            layer_path_t const& most_derived = most_derived_layer(input_layers);
            layer_path_t new_target_layer;
            // Deal with folds and unfolds
            if (node.spec->type == node_type::fold) {
                auto const end = ranges::find(most_derived, node.spec->target_layer_name);
                if (end == most_derived.cend()) {
                    throw std::runtime_error(fmt::format(
                          "Target layer {} of {} does not appear in most derived path: {}",
                          node.spec->target_layer_name.value(), node, fmt_lp(most_derived)));
                }
                new_target_layer = layer_path_t(most_derived.cbegin(), ranges::next(end));
            }
            else if (node.spec->type == node_type::unfold) {
                new_target_layer = most_derived;
                new_target_layer.push_back(node.spec->target_layer_name.value());
            }
            else {
                new_target_layer = most_derived;
            }
            if (new_target_layer == node.target_layer) {
                // no change
                fmt::print("Target layer of {} remains {}\n", node, fmt_lp(node.target_layer));
                continue;
            }
            fmt::print("Target layer of {} is changing from {} to {}\n", node,
                       fmt_lp(node.target_layer), fmt_lp(new_target_layer));
            node.target_layer = std::move(new_target_layer);
            modified = true;
        }
    }
    return modified;
}

bool validate_nodes(node_list_t const& nodes) {
    fmt::print("Validating nodes\n");
    for (node const& node : nodes) {
        if (!node.validate()) {
            fmt::print("{} didn't validate\n", node);
            return false;
        }
    }
    return true;
}

Graph calculate(std::vector<init_prod> const& initial_products,
                std::vector<node_spec> const& algorithms) {
    // Make lists to store all products and nodes
    product_list_t product_list{};
    node_list_t node_list{};
    node_spec_list_t provider_spec_list{};

    make_initial_products(initial_products, product_list, node_list, provider_spec_list);
    // Check providers all validate
    assert(ranges::all_of(node_list, &node::validate) && "Provider nodes didn't all validate");
    // Now we actually make the rest of the nodes
    make_nodes(algorithms, node_list, product_list);

    fmt::print("Now repeatedly connect nodes until they all validate.\n");
    int pass = 0;
    bool modified = true;
    bool validated = true;
    do {
        fmt::print(fmt::fg(fmt::terminal_color::green), "Pass {} -- connecting inputs\n", ++pass);
        modified = connect_inputs(node_list, product_list);
        fmt::print(fmt::fg(fmt::terminal_color::green), "Pass {} -- setting layers\n", pass);
        modified |= update_layers(node_list, product_list);
    } while (modified);
    if (not validate_nodes(node_list)) {
        fmt::print(fmt::fg(fmt::color::red),
                   "Not modified on last pass but nodes don't all validate!\n");
        validated = false;
    }
    else {
        fmt::print(fmt::fg(fmt::terminal_color::green), "DONE!!!\n");
    }

    // Let's make the graph
    Graph graph;

    std::unordered_map<node const*, Graph::vertex_descriptor> node_map{};
    // Add all the vertices
    for (node const& node : node_list) {
        auto v = boost::add_vertex(graph);
        graph[v].name = node.spec->name;
        graph[v].type = node.spec->type;
        graph[v].layer = node.target_layer;
        node_map[&node] = v;
    }

    // Now the edges by going through each node and adding an edge from the creator of each input to
    // this node
    for (node const& node : node_list) {
        for (auto const& [query, p] : views::zip(node.spec->input_queries, node.inputs)) {
            if (p == nullptr) {
                // Add vertex for missing inputs
                auto v = boost::add_vertex(graph);
                graph[v].name = query.name;
                graph[v].type = node_type::provider;
                graph[v].missing = true;
                auto const any = "ANY"_id;
                auto const any_job = "any job"_id;
                auto const current = "current job"_id;
                auto const earlier = "earlier job"_id;
                graph[v].comment = fmt::format("{} ⇦ {} ∈ {} @ {}", query.name,
                                               query.creator_name.value_or(any),
                                               query.layer_name.value_or(any),
                                               query.stage == stage_t::any       ? any_job
                                               : query.stage == stage_t::earlier ? earlier
                                                                                 : current);
                auto [e, _] = boost::add_edge(v, node_map[&node], graph);
                graph[e].name = query.name;
            }
            else {
                auto [e, _] = boost::add_edge(node_map[p->creator], node_map[&node], graph);
                graph[e].name = p->name;
            }
        }
    }

    if (validated) {
        fmt::print(fmt::fg(fmt::terminal_color::green), "Running cycle detection:\n");
        for (Subgraph const& sg : connected_components(graph)) {
            if (!is_dag(sg)) {
                fmt::print(fmt::fg(fmt::color::red), " [ERROR] Found non-DAG component\n");
                exit(1);
            }
        }
    }

    return graph;
}
