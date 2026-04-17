#include "calculate.h"
#include "definitions.h"
#include "layer_path_t.h"

#include <fstream>

#include <boost/graph/graphviz.hpp>

auto main(int argc, char* argv[]) -> int {
    if (argc != 2) {
        fmt::print("Usage: {} graph.dot\n", argv[0]);
        return 1;
    }

    layer_path_t run = "/job/run"_lp;
    layer_path_t subrun = "/job/run/subrun"_lp;
    layer_path_t event = "/job/run/subrun/event"_lp;

    std::vector<init_prod> initial_products{
          {.name = "number", .creator = "provide_geometry", .layer = run},
          {.name = "another", .creator = "provide_string", .layer = subrun},
          {.name = "still", .creator = "provide_integer", .layer = event}};

    std::vector<node_spec> nodes{
          {.type = node_type::transform,
           .name = "A1",
           .output_names = {"a1_output"},
           .input_queries = {{.name = "number",
                              .creator_name = "provide_geometry",
                              .layer_name = "run"}}},
          {.type = node_type::transform,
           .name = "A2",
           .output_names = {"a2_output"},
           .input_queries = {{.name = "a1_output", .creator_name = "A1", .layer_name = "run"}}},
          {.type = node_type::transform,
           .name = "A3",
           .output_names = {"a3_output"},
           .input_queries = {{.name = "a2_output", .creator_name = "A2", .layer_name = "run"}}},

          {.type = node_type::transform,
           .name = "B1",
           .output_names = {"b1_output"},
           .input_queries = {{.name = "a1_output", .creator_name = "A1", .layer_name = "run"},
                             {.name = "another",
                              .creator_name = "provide_string",
                              .layer_name = "subrun"}}},
          {.type = node_type::transform,
           .name = "B2",
           .output_names = {"b2_output"},
           .input_queries = {{.name = "a2_output", .creator_name = "A2", .layer_name = "run"},
                             {.name = "b1_output", .creator_name = "B1", .layer_name = "subrun"}}},

          {.type = node_type::transform,
           .name = "C",
           .output_names = {"c_output"},
           .input_queries = {
                 {.name = "b2_output", .creator_name = "B2", .layer_name = "subrun"},
                 {.name = "still", .creator_name = "provide_integer", .layer_name = "event"}}}};

    Graph graph = calculate(initial_products, nodes);
    std::ofstream outfile(argv[1]);
    boost::write_graphviz(outfile, graph, node_writer(graph), edge_writer(graph), graph_writer);
    return 0;
}
