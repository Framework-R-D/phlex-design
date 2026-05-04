#include "calculate.h"
#include "definitions.h"
#include "layer_path_t.h"

#include <fstream>
#include <vector>

#include <boost/graph/graphviz.hpp>
#include <fmt/format.h>

// This test exercises branchwise fanout for a single declared node template.
//
// The initial products define two distinct concrete layers with the same local
// name:
//
//   /job/run/spill
//   /job/mc_sample/spill
//
// Each node below is declared only once and refers only to the local layer name
// "spill". The solver should therefore instantiate each declared node once for
// the run spill and once for the mc_sample spill.
//
// In particular, this is not testing two hand-written nodes such as
// calibrate_run_spill and calibrate_mc_spill. It is testing that one node spec
// fans out over both compatible concrete spill layers.
auto main(int argc, char* argv[]) -> int {
    if (argc != 2) {
        fmt::print("Usage: {} graph.dot\n", argv[0]);
        return 1;
    }

    std::vector<init_prod> initial_products{
          {.name = "raw_spill_hits", .creator = "daq", .layer = "/job/run/spill"_lp},
          {.name = "raw_spill_hits", .creator = "simulation", .layer = "/job/mc_sample/spill"_lp},
          {.name = "geometry", .creator = "geom_db", .layer = "/job"_lp},
    };

    std::vector<node_spec> nodes;

    // One declared transform. Expected concrete executions:
    //   calibrate_spill_hits @ /job/run/spill
    //   calibrate_spill_hits @ /job/mc_sample/spill
    nodes.push_back(
          {.type = node_type::transform,
           .name = "calibrate_spill_hits",
           .output_names = {"calibrated_spill_hits"},
           .input_queries =
                 {
                       {.name = "raw_spill_hits", .layer_name = "spill"},
                       {.name = "geometry"},
                 }});

    // One declared downstream transform. It should also fan out over both spill
    // layers by consuming the products created by the previous single node spec.
    nodes.push_back(
          {.type = node_type::transform,
           .name = "make_spill_occupancy",
           .output_names = {"spill_occupancy"},
           .input_queries =
                 {
                       {.name = "calibrated_spill_hits",
                        .creator_name = "calibrate_spill_hits",
                        .layer_name = "spill"},
                       {.name = "geometry"},
                 }});

    // One declared consumer. It should consume both concrete spill_occupancy
    // products, again by matching the local layer name "spill".
    nodes.push_back(
          {.type = node_type::consumer,
           .name = "write_spill_occupancy",
           .input_queries =
                 {
                       {.name = "spill_occupancy",
                        .creator_name = "make_spill_occupancy",
                        .layer_name = "spill"},
                 }});

    Graph graph = calculate(initial_products, nodes);

    std::ofstream outfile(argv[1]);
    boost::write_graphviz(outfile, graph, node_writer(graph), edge_writer(graph), graph_writer);
    return 0;
}
