#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "identifier.hpp"
#include "layer_path_t.h"

#include <optional>
#include <source_location>
#include <vector>

using namespace phlex::literals;

using id = phlex::identifier;
using opt_id = std::optional<id>;
enum class stage_t : char { any = 0, earlier = 1, current = 2 };

// Other types
// Type used to specify input products
struct input_query {
    id name; // Will be a combination of name and concept, hence mandatory
    opt_id creator_name;
    opt_id layer_name;
    stage_t stage = stage_t::any;
    std::source_location src_loc = std::source_location::current();

    bool validate() const;
};

// Type specifying initial input products
struct init_prod {
    id name;
    id creator;
    layer_path_t layer;
    std::source_location src_loc = std::source_location::current();

    bool validate() const;
};

struct node; // forward declare the node type so we can hold a pointer to it

// Type representing a product internally
struct product {
    id const name;
    bool current_job;
    node const* creator;
    stage_t stage() const { return current_job ? stage_t::current : stage_t::earlier; }
    layer_path_t const& layer() const;

    id const& creator_name() const;
    // Do we have a full layer path (i.e. one beginning with "job")
    bool has_layer_path() const { return layer().size() >= 1 && layer().front() == "job"_idq; }
    // Is this product fully filled in (i.e. it has a full layer path and is connected to its
    // creator)
    bool filled_in() const { return creator && has_layer_path(); }
};
std::string format_as(product const& p);

// Consumers here cover both observers and preservers
enum class node_type : char { transform = 1, fold, unfold, provider, consumer };
std::string_view format_as(node_type t);
// Type used to specify a node
struct node_spec {
    node_type const type; // Must not be set to "provider". Providers are defined by entries in
                          // initial product list
    id const name;
    std::optional<id> target_layer_name; // Must be provided for folds and unfolds, otherwise must
                                         // not be provided
    std::vector<id> output_names{}; // Must be provided unless this is a consumer, in which case
                                    // this must be empty
    std::vector<input_query> input_queries{}; // Must not be empty
    std::source_location src_loc = std::source_location::current();

    bool validate() const;
};

// Type representing a node internally
struct node {
    node_spec const* const spec;
    std::vector<product const*> inputs{};

    std::vector<product const*> outputs{};
    layer_path_t target_layer;
    // Do we have a full layer path (i.e. one beginning with "job")
    bool has_layer_path() const {
        return target_layer.size() >= 1 && target_layer.front() == "job"_idq;
    }
    bool outputs_filled() const { return outputs.size() == spec->output_names.size(); }
    bool inputs_connected() const;
    bool filled_in() const { return outputs_filled() and inputs_connected() and has_layer_path(); }

    // Check that we're filled in, and our inputs and outputs are configured properly
    // Strictly, only checks that inputs are connected, not that it's the best solution
    bool validate() const;
};
std::string format_as(node const& n);

#endif // DEFINITIONS_H_
