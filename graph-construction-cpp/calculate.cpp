#include "calculate.h"

#include <algorithm>
#include <list>
#include <ranges>
#include <vector>

#include <fmt/format.h>

// Declare ProductList and NodeList concepts we can use in the calculation functions later
template <typename R>
concept ProductList = std::ranges::input_range<R>
                      && std::same_as<std::ranges::range_value_t<R>, product>;
template <typename R>
concept NodeList = std::ranges::input_range<R> && std::same_as<std::ranges::range_value_t<R>, node>;

void make_initial_products(std::vector<init_prod> const& initial_products,
                           std::list<product>& product_list, std::list<node>& node_list,
                           std::list<node_spec>& provider_spec_list) {
    assert(product_list.empty() && node_list.empty() && provider_spec_list.empty());
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

        node& provider = node_list.emplace_back(&provider_spec, prod.layer);
        product& new_prod = product_list.emplace_back(product{
              .name = prod.name, .current_job = false, .creator = &provider, .layer = prod.layer});

        provider.outputs.push_back(&new_prod);
    }
}

void make_nodes(std::vector<node_spec> const& algorithms, std::list<node>& node_list) {
    // Fully populate the node list, though these won't be filled in yet
    for (const auto& spec : algorithms) {
        node& new_node = node_list.emplace_back(&spec);
        // Check we have a target layer name if this is a fold or unfold
        if ((spec.type == node_type::fold or spec.type == node_type::unfold)
            and !spec.target_layer_name.has_value()) {
            throw std::logic_error(fmt::format("The {} requires a target layer name!", new_node));
        }
    }
}

Graph calculate(std::vector<init_prod> const& initial_products,
                std::vector<node_spec> const& algorithms) {
    // Make lists to store all products and nodes
    // Use lists for pointer stability (re-evaluate later for efficiency)
    std::list<product> product_list{};
    std::list<node> node_list{};
    std::list<node_spec> provider_spec_list{};

    make_initial_products(initial_products, product_list, node_list, provider_spec_list);
    // At this point all products in product_list are initial products, so we'll make two views, one
    // with products from earlier stages, and one with products from the current stage. These will
    // take the form of lambdas which return a view.
    auto earlier_products = [count = product_list.size(), &product_list] {
        return std::views::take(product_list, count);
    };
    auto current_products = [count = product_list.size(), &product_list] {
        return std::views::drop(product_list, count);
    };
  // We can do something similar with the node lists
  // (first check that n_nodes == n_products)

    return Graph{};
}
