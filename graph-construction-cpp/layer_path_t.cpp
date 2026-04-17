#include "layer_path_t.h"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <boost/container_hash/hash.hpp>
#include <algorithm>
#include <ranges>
#include <string_view>

std::string fmt_lp(layer_path_t const& lp) { return fmt::format("/{}", fmt::join(lp, "/")); }

layer_path_t operator""_lp(char const* lit, std::size_t size) {
    using namespace std::string_view_literals;
    std::string_view lit_sv(lit, size);
    if (lit_sv.empty()) {
        return {};
    }
    bool const drop_first = lit_sv[0] == '/';
    return lit_sv | std::views::split("/"sv) | std::views::drop(drop_first ? 1 : 0)
           | std::views::transform(
                 [](auto const& r) { return phlex::identifier(std::string_view(r)); })
           | std::ranges::to<layer_path_t>();
}

layer_path_t common_prefix(layer_path_t const& lp1, layer_path_t const& lp2) {
    auto [it, _] = std::ranges::mismatch(lp1, lp2);
    if (it == lp1.cbegin()) {
        return {};
    }

    return layer_path_t(lp1.cbegin(), it);
}

bool forms_hierarchy(layer_path_list& layer_paths) {
    if (layer_paths.size() < 2) {
        return true;
    }
    layer_paths.sort(
          [](layer_path_t const& lp1, layer_path_t const& lp2) { return lp1.size() < lp2.size(); });
    return std::ranges::all_of(layer_paths
                                     | std::views::pairwise_transform([](layer_path_t const& lp1,
                                                                         layer_path_t const& lp2) {
                                           // Check that lp2 starts with lp1 ==> mismatch =
                                           // lp1.end()
                                           // libstdc++ doesn't support starts_with yet
                                           return std::ranges::mismatch(lp1, lp2).in1 == lp1.cend();
                                       }),
                               std::identity{});
}

layer_path_t const& most_derived_layer(layer_path_list& layer_paths) {
    assert(forms_hierarchy(layer_paths) && "Tried to find most derived layer of a non-hierarchy");
    // Now it's sorted, so return the last element
    return layer_paths.back();
}

std::size_t hash_lp(layer_path_t const& lp) { return boost::hash_range(lp.cbegin(), lp.cend()); }
