#include "definitions.h"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <boost/container_hash/hash.hpp>
#include <boost/lambda2.hpp>

#include <algorithm>
#include <flat_set>
#include <string_view>

using namespace boost::lambda2;

id const& product::creator_name() const { return creator->spec->name; }
layer_path_t const& product::layer() const { return creator->target_layer; }

std::string format_as(product const& p) {
    return fmt::format("{} ({:04X} CREATED BY {})", p.name, std::uintptr_t(&p) & 0xFFFF,
                       *p.creator);
}

std::string_view format_as(node_type t) {
    using namespace std::string_view_literals;
    switch (t) {
    case node_type::transform:
        return "transform"sv;
    case node_type::fold:
        return "fold"sv;
    case node_type::unfold:
        return "unfold"sv;
    case node_type::provider:
        return "provider"sv;
    case node_type::consumer:
        return "consumer"sv;
    default:
        return "UNKNOWN"sv;
    }
}

std::string format_as(node const& n) {
    return fmt::format("{} {} ({:04X})", n.spec->name, n.spec->type, std::uintptr_t(&n) & 0xFFFF);
}

bool node_spec::validate() const {
    if (name.empty()) {
        return false;
    }
    if (type <= node_type(0) || type > node_type::consumer) {
        return false;
    }
    if (type == node_type::fold || type == node_type::unfold) {
        if (!target_layer_name.has_value()) {
            return false;
        }
    }
    else {
        if (target_layer_name.has_value()) {
            return false;
        }
    }
    if (type != node_type::provider && output_names.empty()) {
        return false;
    }
    if (type != node_type::consumer && input_queries.empty()) {
        return false;
    }
    return true;
}

bool node::inputs_connected() const {
    if (inputs.size() != spec->input_queries.size()) {
        return false;
    }
    return std::ranges::all_of(inputs, _1 != nullptr);
}

bool node::validate() const {
    auto const warn_style = fmt::fg(fmt::color::orange);
    if (!filled_in()) {
        fmt::print(warn_style,
                   "{} isn't filled in. outputs_filled() = {}, inputs_connected()={}, "
                   "has_layer_path()={}, target_layer: {}\n",
                   *this, this->outputs_filled(), this->inputs_connected(), this->has_layer_path(),
                   this->target_layer);
        return false;
    }

    std::flat_set<id> output_names{spec->output_names};

    for (product const* p : outputs) {
        // Remainder are really errors, but let's just warn for now
        // Make sure the creator link points to us
        if (p->creator != this) {
            fmt::print(warn_style, "WARN: {} has output {} with creator set to {}", *this, *p,
                       *(p->creator));
            return false;
        }
        // Make sure this output is listed as one of ours
        if (!output_names.contains(p->name)) {
            fmt::print(warn_style, "WARN: {} has output product {} not listed in output_names",
                       *this, *p);
            return false;
        }
    }
    return true;
}
