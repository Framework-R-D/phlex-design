#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "identifier.hpp"
#include <list>
#include <optional>
#include <vector>

using namespace phlex::literals;

using id = phlex::identifier;
using opt_id = std::optional<id>;
using layer_path_t = std::list<id>;
enum class stage_t : char { any = 0, earlier = 1, current = 2 };

std::string fmt_lp(layer_path_t const& lp);

// Type used to specify input products
struct input_query {
    id name; // Will be a combination of name and concept, hence mandatory
    opt_id creator_name;
    opt_id layer_name;
    stage_t stage;
};

// Type specifying initial input products
struct init_prod {
    id name;
    id creator;
    layer_path_t layer;
};

struct node; // forward declare the node type so we can hold a pointer to it

// Type representing a product internally
struct product {
    id const name;
    bool current_job;
    node const* creator;
    layer_path_t layer;

    id const& creator_name() const;
    // Do we have a full layer path (i.e. one beginning with "job")
    bool has_layer_path() const { return layer.size() >= 1 && layer.front() == "job"_idq; }
    // Is this product fully filled in (i.e. it has a full layer path and is connected to its
    // creator)
    bool filled_in() const { return creator && has_layer_path(); }
};
std::uint64_t hash_value(product const& p);
id format_as(product const& p); // Because identifiers already have a formatter

// Consumers here cover both observers and preservers
enum class node_type : char { transform = 1, fold, unfold, provider, consumer };
// Type used to specify a node
struct node_spec {
    node_type const type;
    id const name;
    std::optional<id> target_layer_name; // optional because this is only relevant for folds and
    // unfolds
    std::vector<id> output_names{};
    std::vector<input_query> input_queries{};
};

// Type representing a node internally
struct node {
    node_spec const* const spec;
    layer_path_t target_layer;
    // Do we have a full layer path (i.e. one beginning with "job")
    bool has_layer_path() const {
        return target_layer.size() >= 1 && target_layer.front() == "job"_idq;
    }
    std::vector<product const*> outputs{};
    std::vector<product const*> inputs{};
    bool outputs_filled() const { return outputs.size() == spec->output_names.size(); }
    bool inputs_connected() const { return inputs.size() == spec->input_queries.size(); }
    bool filled_in() const { return outputs_filled() and inputs_connected() and has_layer_path(); }

    // Check that we're filled in, and our outputs are configured properly
    bool validate() const;
};
std::string format_as(node const& n);

#endif // DEFINITIONS_H_
