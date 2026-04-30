#include "Graph.h"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include <flat_map>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

void node_writer::operator()(std::ostream& out, Graph::vertex_descriptor const& desc) {
    std::string color;
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
        color = "crimson";
        break;
    }
    fmt::print(
          out,
          R"([style="filled,rounded", fillcolor={}, shape=rect, label=<<B>{}</B><BR/><I>{}</I><BR/>{}>])",
          color, graph_[desc].name, graph_[desc].type, fmt_lp(graph_[desc].layer));
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

std::generator<Subgraph const&> connected_components(Graph const& g) {
  auto s = fmt::fg(fmt::terminal_color::green);
    fmt::print(s, "   Calculating connected components... ");
    auto vertex_id_map = get(boost::vertex_index, g);
    boost::vector_property_map<int, decltype(vertex_id_map)> component_map(vertex_id_map);
    int const num_components = boost::connected_components(g, component_map);
    fmt::print(s, "{}\n", num_components);

  std::vector<vertex_filter> filters(num_components);
  for (auto v : boost::vertices(g))
}
