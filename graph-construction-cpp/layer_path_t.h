#ifndef LAYER_PATH_T_H_
#define LAYER_PATH_T_H_

#include "identifier.hpp"
#include <list>
#include <vector>

// Layer path utilities
using layer_path_t = std::vector<phlex::identifier>;
std::string fmt_lp(layer_path_t const& lp);
layer_path_t operator""_lp(char const* lit, std::size_t size);
layer_path_t common_prefix(layer_path_t const& lp1, layer_path_t const& lp2);

using layer_path_list = std::list<std::reference_wrapper<layer_path_t const>>;
// Determine whether some set of layer paths form a hierarchy
bool forms_hierarchy(layer_path_list& layer_paths);
// Given a hierarchy of layer paths, calculate the most derived layer
layer_path_t const& most_derived_layer(layer_path_list& layer_paths);

std::size_t hash_lp(layer_path_t const& lp);

#endif // LAYER_PATH_T_H_
