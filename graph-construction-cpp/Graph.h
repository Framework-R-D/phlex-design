#ifndef GRAPH_H_
#define GRAPH_H_

#include "definitions.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

#include <flat_set>
#include <generator>
#include <ostream>

struct node_props {
    id name;
    node_type type;
    layer_path_t layer;
};

struct prod_props {
    id name;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, node_props,
                                    prod_props, boost::no_property>;
class node_writer {
  public:
    node_writer(Graph const& g) : graph_(g) {}

    void operator()(std::ostream& out, Graph::vertex_descriptor const& desc);

  private:
    Graph const& graph_;
};

class edge_writer {
  public:
    edge_writer(Graph const& g) : graph_(g) {}

    void operator()(std::ostream& out, Graph::edge_descriptor const& desc);

  private:
    Graph const& graph_;
};

void graph_writer(std::ostream& out);

namespace {
class vertex_filter {
  public:
    // Stores list of vertices in this component
    std::flat_set<Graph::vertex_descriptor> vertices{};
    bool operator()(Graph::vertex_descriptor const& v) const noexcept {
        return vertices.contains(v);
    }
};
} // namespace

using Subgraph = boost::filtered_graph<Graph, boost::keep_all, vertex_filter>;

std::generator<Subgraph const&> connected_components(Graph const& g);
bool is_dag(Subgraph const& sg);

#endif // GRAPH_H_
