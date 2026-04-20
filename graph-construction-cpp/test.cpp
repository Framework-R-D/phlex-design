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

    std::vector<init_prod> initial_products{
          {.name = "number", .creator = "input", .layer = "/job/run/event"_lp}};

    std::vector<node_spec> nodes{
          {.type = node_type::fold,
           .name = "run_add",
           .target_layer_name = "run",
           .output_names = {"run_sum"},
           .input_queries = {{.name = "number", .creator_name = "input"}}},
          {.type = node_type::fold,
           .name = "job_add",
           .target_layer_name = "job",
           .output_names = {"job_sum"},
           .input_queries = {{.name = "number", .layer_name = "event"}}},
          {.type = node_type::fold,
           .name = "two_layer_job_add",
           .target_layer_name = "job",
           .output_names = {"two_layer_job_sum"},
           .input_queries = {{.name = "run_sum"}}},
          {.type = node_type::transform,
           .name = "run_increment",
           .output_names = {"run_inc"},
           .input_queries = {{.name = "run_sum"}, {.name = "job_sum"}}},
          {.type = node_type::unfold,
           .name = "run_to_spill",
           .target_layer_name = "spill",
           .output_names = {"number"},
           .input_queries = {{.name = "run_sum"}, {.name = "run_inc"}}},
          {.type = node_type::consumer,
           .name = "verify_run_sum",
           .input_queries = {{.name = "run_sum"}}},
          {.type = node_type::consumer,
           .name = "verify_two_layer_job_add",
           .input_queries = {{.name = "two_layer_job_sum"}}},
          {.type = node_type::consumer,
           .name = "verify_job_add",
           .input_queries = {{.name = "job_sum"}}},
    };

    Graph graph = calculate(initial_products, nodes);
    std::ofstream outfile(argv[1]);
    boost::write_graphviz(outfile, graph, node_writer(graph), edge_writer(graph), graph_writer);
    return 0;
}
