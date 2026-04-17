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

// Declare ProductList and NodeList concepts we can use in the calculation functions later
template <typename R>
concept ProductList = ranges::viewable_range<R> && std::same_as<ranges::range_value_t<R>, product>;
template <typename R>
concept NodeList = ranges::viewable_range<R> && std::same_as<ranges::range_value_t<R>, node>;

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
bool connect_inputs(node& node, ProductList auto const& products) {
    // Reconnect all inputs unconditionally
    int slot = 0;
    int const n_slots = node.spec->input_queries.size();
    bool modified = false;
    for (auto&& [query, link] : views::zip(node.spec->input_queries, node.inputs)) {
        ++slot;
        auto match = [&query = std::as_const(query)](product const& p) {
            // Not strictly necessary, but just in case this function is called without filtering
            // the product list
            if (!p.filled_in()) {
                return false;
            }
            // name is mandatory
            if (p.name != query.name) {
                return false;
            }
            if (query.creator_name && (p.creator->spec->name != query.creator_name)) {
                return false;
            }
            if (query.layer_name && (p.layer().back() != query.layer_name)) {
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
        // Otherwise, things get complicated. Start by only supporting the case where we have
        // products from multiple stages, of which exactly one is from the current job. Later we
        // might also think about the case where we have multiple products from different layers but
        // that requires duplicating nodes or allowing nodes and products to have multiple layers.
        // Count distinct stages by making a set and getting its size
        int const distinct_stages = (views::transform(matching_products,
                                                      [](product const* p) { return p->stage(); })
                                     | ranges::to<std::flat_set>())
                                          .size();
        if (distinct_stages <= 1) {
            // This is an error for now
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
    if (modified) {
        auto input_layers = node.inputs | views::transform([](product const* p) {
                                return std::cref(p->layer());
                            })
                            | ranges::to<layer_path_list>();
        bool const hierarchy_ok = forms_hierarchy(input_layers);
        if (!hierarchy_ok) {
            auto input_layer_names = input_layers
                                     | views::transform([](auto const& lp) { return fmt_lp(lp); })
                                     | ranges::to<std::vector<std::string>>();
            throw std::runtime_error(
                  fmt::format("Reconnected inputs for {} but input layers do not form a hierarchy. "
                              "Input layers are:\n  - {}\n",
                              node, fmt::join(input_layer_names, "\n  - ")));
        }
        layer_path_t const& new_layer = most_derived_layer(input_layers);
        if (node.has_layer_path()) {
            if (node.target_layer != new_layer) {
                fmt::print("Target layer of {} is changing from {} to {}\n", node,
                           node.target_layer, new_layer);
                node.target_layer = new_layer;
            }
        }
        node.target_layer = new_layer;
    }
    return modified;
}

bool connect_inputs(NodeList auto& nodes, ProductList auto const& products) {
    // Connect any inputs that we can now resolve
    fmt::print("Connecting inputs\n");
    bool modified = false;
    for (node& node : nodes) {
        modified |= connect_inputs(node, products);
    }
    return modified;
}

bool validate_nodes(NodeList auto const& nodes) {
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
    // At this point all products in product_list are initial products, so we'll make two views, one
    // with products from earlier stages, and one with products from the current stage. These will
    // take the form of lambdas which return a view.
    auto earlier_products = [count = product_list.size(), &product_list] {
        return views::take(product_list, count);
    };
    auto current_products = [count = product_list.size(), &product_list] {
        return views::drop(product_list, count);
    };
    // We can do something similar with the node lists, separating the providers from the
    // computational nodes
    auto providers = [count = node_list.size(), &node_list] {
        return views::take(node_list, count);
    };
    // Check providers all validate
    assert(ranges::all_of(providers(), &node::validate) && "Provider nodes didn't all validate");
    auto compute_nodes = [count = node_list.size(), &node_list] {
        return views::drop(node_list, count);
    };
    // Now we actually make the rest of the nodes
    make_nodes(algorithms, node_list, product_list);

    fmt::print("Now repeatedly connect nodes until they all validate.\n");
    int pass = 0;
    bool modified = true;
    do {
        fmt::print(fmt::fg(fmt::color::green), "Pass {}\n", ++pass);
        if (!modified) {
          fmt::print(fmt::fg(fmt::color::red), "Warning: Not modified on last pass\n");
        }
        modified = connect_inputs(node_list, product_list);
    } while (not validate_nodes(node_list));
    fmt::print("DONE!!!\n");

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
        for (product const* p : node.inputs) {
            auto [e, _] = boost::add_edge(node_map[p->creator], node_map[&node], graph);
            graph[e].name = p->name;
        }
    }

    return graph;
}
