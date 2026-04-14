#include "definitions.h"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <boost/container_hash/hash.hpp>

#include <flat_set>

std::string fmt_lp(layer_path_t const& lp) { return fmt::format("/{}", fmt::join(lp, "/")); }

id const& product::creator_name() const { return creator ? creator->spec->name : id::blank; }

std::uint64_t hash_value(product const& p) {
    std::uint64_t result = p.name.hash();
    if (p.creator) {
        boost::hash_combine(result, *p.creator);
    }
    else {
        boost::hash_combine(result, 0);
    }
    boost::hash_combine(result, p.layer);
    return result;
}

id format_as(product const& p) { return p.name; }

std::string format_as(node const& n) {
    using namespace std::string_view_literals;
    std::string_view type_sv;
    switch (n.spec->type) {
    case node_type::transform:
        type_sv = "transform"sv;
        break;
    case node_type::fold:
        type_sv = "fold"sv;
        break;
    case node_type::unfold:
        type_sv = "unfold"sv;
        break;
    case node_type::provider:
        type_sv = "provider"sv;
        break;
    case node_type::consumer:
        type_sv = "consumer"sv;
        break;
    }
    return fmt::format("{} {} ({:04X})", n.spec->name, type_sv, std::uintptr_t(&n));
}

bool node::validate() const {
    auto const warn_style = fmt::emphasis::bold | fmt::fg(fmt::color::red);
    if (!filled_in()) {
        return false;
    }

    std::flat_set<id> output_names{spec->output_names};

    for (product const* p : outputs) {
        // Make sure product is filled in
        if (!p->filled_in()) {
            return false;
        }
        // Remainder are really errors, but let's just warn for now
        // Make sure the creator link points to us
        if (p->creator != this) {
            fmt::print(warn_style, "WARN: {} has output {} with creator set to {}", *this, p,
                       *(p->creator));
            return false;
        }
        // Make sure this output is listed as one of ours
        if (!output_names.contains(p->name)) {
            fmt::print(warn_style, "WARN: {} has output product {} not listed in output_names",
                       *this, p);
            return false;
        }
        // Make sure product's layer path matches ours
        if (p->layer != target_layer) {
            fmt::print(warn_style, "WARN: {} has output {} with layer {} != target layer {}", *this,
                       p, fmt_lp(p->layer), fmt_lp(target_layer));
            return false;
        }
    }
    return true;
}
