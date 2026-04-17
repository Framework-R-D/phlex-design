#ifndef GRAPH_H_
#define GRAPH_H_

#include "definitions.h"

#include <boost/graph/adjacency_list.hpp>

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

#endif // GRAPH_H_
