#include "Graph.h"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/topological_sort.hpp>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <ranges>

namespace {
template <typename Iter, typename Sent> auto pair_to_range(std::pair<Iter, Sent> in) {
    return std::ranges::subrange<Iter, Sent>(in.first, in.second);
}
} // namespace

void node_writer::operator()(std::ostream& out, Graph::vertex_descriptor const& desc) {
    std::string color;
    if (graph_[desc].missing) {
        color = "crimson";
        fmt::print(
              out,
              R"([style="filled,rounded", fillcolor={}, shape=rect, label=<<B>{}</B><BR/><B><I>MISSING</I></B>: {}<BR/>{}>])",
              color, graph_[desc].name, graph_[desc].comment, fmt_lp(graph_[desc].layer));
    }
    else {
        switch (graph_[desc].type) {
        case node_type::provider:
            color = "chartreuse";
            break;
        case node_type::transform:
            color = "cadetblue1";
            break;
        case node_type::fold:
            color = "gold";
            break;
        case node_type::unfold:
            color = "silver";
            break;
        case node_type::consumer:
            color = "turquoise";
            break;
        }
        fmt::print(
              out,
              R"([style="filled,rounded", fillcolor={}, shape=rect, label=<<B>{}</B><BR/><I>{}</I><BR/>{}>])",
              color, graph_[desc].name, graph_[desc].type, fmt_lp(graph_[desc].layer));
    }
}

void edge_writer::operator()(std::ostream& out, Graph::edge_descriptor const& desc) {
    fmt::print(out, R"([label="{}"])", graph_[desc].name);
}

void graph_writer(std::ostream& out) {
    fmt::print(out, R"(graph [bgcolor=white, fontname="Iosevka,Monospace"])"
                    "\n");
    fmt::print(out, R"(node [style="filled,rounded", shape=rect, fontname="Iosevka,Monospace"])"
                    "\n");
    fmt::print(out, R"(edge [fontname="Iosevka,Monospace"])"
                    "\n");
}

std::generator<Subgraph> connected_components(Graph const& g) {
    auto s = fmt::fg(fmt::terminal_color::green);
    fmt::print(s, "   Calculating connected components... ");
    UndirGraph ug(boost::num_vertices(g));
    for (auto e : pair_to_range(boost::edges(g))) {
        boost::add_edge(boost::source(e, g), boost::target(e, g), ug);
    }
    auto vertex_id_map = get(boost::vertex_index, ug);
    boost::vector_property_map<int, decltype(vertex_id_map)> component_map(vertex_id_map);
    int const num_components = boost::connected_components(ug, component_map);
    fmt::print(s, "{}\n", num_components);

    std::vector<vertex_filter> filters(num_components);
    for (auto v : pair_to_range(boost::vertices(g))) {
        filters[component_map[v]].vertices.insert(v);
    }
    for (auto&& vf : filters) {
        co_yield Subgraph(g, std::move(boost::keep_all{}), std::move(vf));
    }
}

bool is_dag(Subgraph const& sg) {
    auto s = fmt::fg(fmt::terminal_color::green);
    auto w = fmt::fg(fmt::terminal_color::yellow);
    int const nv = boost::num_vertices(sg);
    if (nv <= 0) {
        fmt::print(w, "[WARN] Subgraph has n_vertices <= 0\n");
        return false;
    }
    if (nv == 1) {
        return true;
    }

    std::list<Subgraph::vertex_descriptor> null{}; // discarded
    try {
        boost::topological_sort(sg, std::back_insert_iterator(null));
        // fmt::print(s, "Sorted: {}\n",
        //            null | std::views::reverse
        //                  | std::views::transform([&sg](Subgraph::vertex_descriptor v) {
        //                        return fmt::format("{}({})", sg[v].type, sg[v].name);
        //                    }));
        return true;
    }
    catch (boost::not_a_dag const&) {
        return false;
    }
}
