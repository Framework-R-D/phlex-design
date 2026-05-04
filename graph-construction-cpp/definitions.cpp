#include "definitions.h"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <boost/container_hash/hash.hpp>
#include <boost/lambda2.hpp>

#include <algorithm>
#include <flat_set>
#include <ranges>
#include <string_view>

using namespace boost::lambda2;

namespace {
auto const warn_style = fmt::fg(fmt::color::orange);
}

id const& product::creator_name() const { return creator->spec->name; }
layer_path_t const& product::layer() const { return creator->target_layer; }

bool input_query::validate() const {
    if (!name.empty()) {
        return true;
    }
    else {
        fmt::print(warn_style, "An input query had an empty name. [{} LINE {}]\n",
                   src_loc.file_name(), src_loc.line());
        return false;
    }
}

bool init_prod::validate() const {
    if (name.empty()) {
        fmt::print(warn_style, "An initial product has a missing name. [{} LINE {}]\n",
                   src_loc.file_name(), src_loc.line());
        return false;
    }
    if (creator.empty() or layer.empty()) {
        fmt::print(warn_style, "The initial product {0} is missing {3}{5}{4}. [{1} LINE {2}]\n",
                   name, src_loc.file_name(), src_loc.line(), creator.empty() ? "a creator" : "",
                   layer.empty() ? "a layer" : "",
                   creator.empty() and layer.empty() ? " and " : "");
        return false;
    }

    if (layer.front() != "job"_idq) {
        fmt::print(
              warn_style,
              "The initial product {} has a layer path that isn't a child of /job. [{} LINE {}]\n",
              name, src_loc.file_name(), src_loc.line());
        return false;
    }
    return true;
}

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
        fmt::print(warn_style, "A node specification has an empty name. [{} LINE {}]\n",
                   src_loc.file_name(), src_loc.line());
        return false;
    }
    if (type <= node_type(0) || type > node_type::consumer) {
        fmt::print(warn_style, "The node specification {} has an invalid type. [{} LINE {}]\n",
                   name, src_loc.file_name(), src_loc.line());
        return false;
    }
    if (type == node_type::fold || type == node_type::unfold) {
        if (!target_layer_name.has_value()) {
            fmt::print(warn_style, "The {} {} has no target layer name. [{} LINE {}]\n", name, type,
                       src_loc.file_name(), src_loc.line());
            return false;
        }
    }
    else {
        if (target_layer_name.has_value()) {
            fmt::print(warn_style, "The {} {} must not have a target layer name. [{} LINE {}]\n",
                       name, type, src_loc.file_name(), src_loc.line());
            return false;
        }
    }
    if (type == node_type::provider && output_names.empty()) {
        fmt::print(warn_style, "The {} {} must have output names. [{} LINE {}]\n", name, type,
                   src_loc.file_name(), src_loc.line());
        return false;
    }
    if (type == node_type::consumer && input_queries.empty()) {
        fmt::print(warn_style, "The {} {} must have input queries. [{} LINE {}]\n", name, type,
                   src_loc.file_name(), src_loc.line());
        return false;
    }
    return true;
}

bool node::inputs_connected() const {
    if (inputs.size() != spec->input_queries.size()) {
        return false;
    }
    if (!std::ranges::all_of(inputs, _1 != nullptr)) return false;
    return std::ranges::all_of(inputs, [](product const* p) { return p->filled_in(); });
}

bool node::validate() const {
    if (!filled_in()) {
        if (!outputs_filled()) {
            throw std::logic_error(
                  fmt::format("ERR: The {} doesn't have it's outputs filled.\n", *this));
        }
        if (!inputs_connected()) {
            fmt::print(warn_style, "The {} has unconnected inputs:\n", *this);
            for (auto const& [i, input] : std::views::enumerate(inputs)) {
                if (!input) {
                    auto const& query = spec->input_queries.at(i);
                    fmt::print(
                          warn_style, "  - {}{}{}\n", query.name,
                          query.creator_name
                                .transform(
                                      [](auto const& id) { return fmt::format(" from {}", id); })
                                .value_or(std::string()),
                          query.layer_name
                                .transform([](auto const& id) { return fmt::format(" in {}", id); })
                                .value_or(std::string()));
                }
            }
        }
        if (!has_layer_path()) {
            fmt::print(warn_style, "The {} has an invalid layer path: {}.\n", *this,
                       fmt_lp(target_layer));
        }
        return false;
    }

    std::flat_set<id> output_names{spec->output_names};

    for (product const* p : outputs) {
        // Remainder are really errors, but let's just warn for now
        // Make sure the creator link points to us
        if (p->creator != this) {
            fmt::print(warn_style, "WARN: {} has output {} with creator set to {}.", *this, *p,
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
