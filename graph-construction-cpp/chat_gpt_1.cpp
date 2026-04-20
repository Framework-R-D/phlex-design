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
          {.name = "raw_hits", .creator = "daq", .layer = "/job/run/spill"_lp},
          {.name = "raw_hits", .creator = "sim", .layer = "/job/run/spill"_lp},

          {.name = "geometry", .creator = "geom_db", .layer = "/job"_lp},

          {.name = "conditions", .creator = "cond_db", .layer = "/job/run"_lp},
          {.name = "run_metadata", .creator = "run_db", .layer = "/job/run"_lp},
          {.name = "run_summary", .creator = "run_db", .layer = "/job/run"_lp},
    };

    std::vector<node_spec> nodes{
          {.type = node_type::consumer,
           .name = "final_sink",
           .input_queries =
                 {
                       {.name = "analysis_table", .creator_name = "unfold_analysis"},
                       {.name = "monitoring", .creator_name = "unfold_analysis"},
                       {.name = "run_summary"},
                 }},

          {.type = node_type::fold,
           .name = "fold_analysis",
           .target_layer_name = "run",
           .output_names = {"analysis_table", "monitoring"},
           .input_queries =
                 {
                       {.name = "analysis_table", .creator_name = "make_analysis_table"},
                       {.name = "monitoring", .creator_name = "select_candidates"},
                 }},

          {.type = node_type::transform,
           .name = "make_analysis_table",
           .output_names = {"analysis_table"},
           .input_queries =
                 {
                       {.name = "candidates", .creator_name = "select_candidates"},
                       {.name = "weights", .creator_name = "compute_weights"},
                       {.name = "run_metadata", .stage = stage_t::earlier},
                 }},

          {.type = node_type::transform,
           .name = "compute_weights",
           .output_names = {"weights"},
           .input_queries =
                 {
                       {.name = "candidates", .creator_name = "select_candidates"},
                       {.name = "conditions"},
                       {.name = "run_summary"},
                 }},

          {.type = node_type::transform,
           .name = "select_candidates",
           .output_names = {"candidates", "monitoring"},
           .input_queries =
                 {
                       {.name = "fitted_tracks", .creator_name = "fit_tracks"},
                       {.name = "conditions"},
                 }},

          {.type = node_type::transform,
           .name = "fit_tracks_alt",
           .output_names = {"fitted_tracks"},
           .input_queries =
                 {
                       {.name = "seed_tracks"},
                       {.name = "conditions"},
                 }},

          {.type = node_type::transform,
           .name = "fit_tracks",
           .output_names = {"fitted_tracks"},
           .input_queries =
                 {
                       {.name = "seed_tracks"},
                       {.name = "conditions"},
                 }},

          {.type = node_type::transform,
           .name = "seed_tracks",
           .output_names = {"seed_tracks"},
           .input_queries =
                 {
                       {.name = "clustered_hits", .creator_name = "cluster_hits_sim"},
                       {.name = "geometry"},
                 }},

          {.type = node_type::transform,
           .name = "cluster_hits_sim",
           .output_names = {"clustered_hits"},
           .input_queries =
                 {
                       {.name = "raw_hits", .creator_name = "sim"},
                       {.name = "geometry"},
                 }},

          {.type = node_type::transform,
           .name = "cluster_hits_daq",
           .output_names = {"clustered_hits"},
           .input_queries =
                 {
                       {.name = "raw_hits", .creator_name = "daq"},
                       {.name = "geometry"},
                 }},

          {.type = node_type::unfold,
           .name = "unfold_analysis",
           .target_layer_name = "analysis_view",
           .output_names = {"analysis_table", "monitoring"},
           .input_queries =
                 {
                       {.name = "analysis_table", .creator_name = "fold_analysis"},
                       {.name = "monitoring", .creator_name = "fold_analysis"},
                 }},

          {.type = node_type::consumer,
           .name = "debug_sink",
           .input_queries =
                 {
                       {.name = "clustered_hits", .creator_name = "cluster_hits_sim"},
                       {.name = "fitted_tracks", .creator_name = "fit_tracks"},
                       {.name = "analysis_table", .creator_name = "make_analysis_table"},
                 }},
    };

    Graph graph = calculate(initial_products, nodes);
    std::ofstream outfile(argv[1]);
    boost::write_graphviz(outfile, graph, node_writer(graph), edge_writer(graph), graph_writer);
    return 0;
}

