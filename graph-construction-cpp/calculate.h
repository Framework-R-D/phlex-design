#include "definitions.h"

#include <boost/graph/adjacency_list.hpp>

struct node_props {
    id name;
    node_type type;
};

struct prod_props {
    id name;
    id creator;
    layer_path_t layer;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, node_props,
                                    prod_props, boost::no_property>;

Graph calculate(std::vector<init_prod> const& initial_products,
                std::vector<node_spec> const& algorithms);
