#include "make_parameter_set_from_YAML.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"
#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

#include <format>
#include <iostream>
#include <sstream>

namespace {

  void add_node_as_map(std::string const& key,
                       YAML::Node const& node,
                       fhicl::intermediate_table& tbl);
  void add_node_as_sequence(std::string const& key,
                            YAML::Node const& node,
                            fhicl::intermediate_table& tbl);
  void add_node_to_table(std::string const& key,
                         YAML::Node const& node,
                         fhicl::intermediate_table& tbl);

  void add_node_as_map(std::string const& key,
                       YAML::Node const& node,
                       fhicl::intermediate_table& tbl)
  {
    tbl.putEmptyTable(key);
    for (auto it = node.begin(); it != node.end(); ++it) {
      add_node_to_table(std::format("{}.{}", key, it->first.Scalar()), it->second, tbl);
    }
  }

  void add_node_as_sequence(std::string const& key,
                            YAML::Node const& node,
                            fhicl::intermediate_table& tbl)
  {
    tbl.putEmptySequence(key);
    size_t idx{0ul};
    for (auto it = node.begin(); it != node.end(); ++it) {
      add_node_to_table(std::format("{}[{}]", key, idx++), *it, tbl);
    }
  }

  void add_node_to_table(std::string const& key,
                         YAML::Node const& node,
                         fhicl::intermediate_table& tbl)
  {
    switch (node.Type()) {
    case YAML::NodeType::Undefined:
      spdlog::info("found undefined {}", key);
      break;
    case YAML::NodeType::Null:
      spdlog::info("found null {}", key);
      tbl.putNil(key);
      break;
    case YAML::NodeType::Scalar: {
      auto const input_string = node.Scalar();
      spdlog::info("found scalar {}: {}", key, input_string);
      fhicl::extended_value xval;
      std::string unparsed;
      if (fhicl::parse_value_string(input_string, xval, unparsed)) {
        tbl.insert(key, std::move(xval));
      } else {
        tbl.put(key, input_string, false);
      }
    } break;
    case YAML::NodeType::Sequence:
      spdlog::info("found sequence {}", key);
      add_node_as_sequence(key, node, tbl);
      break;
    case YAML::NodeType::Map:
      spdlog::info("found map {}", key);
      add_node_as_map(key, node, tbl);
      break;
    default:
      break;
    }
  }

  fhicl::ParameterSet make_parameter_set_from_YAML(YAML::Node const& top_node)
  {
    fhicl::intermediate_table tbl;
    for (auto it = top_node.begin(); it != top_node.end(); ++it) {
      add_node_to_table(it->first.Scalar(), it->second, tbl);
    }
    return fhicl::ParameterSet::make(tbl);
  }
}

fhicl::ParameterSet make_parameter_set_from_YAML_file(std::string const& yaml_filename)
{
  return make_parameter_set_from_YAML(YAML::LoadFile(yaml_filename));
}

fhicl::ParameterSet make_parameter_set_from_YAML_string(std::string const& yaml_in)
{
  return make_parameter_set_from_YAML(YAML::Load(yaml_in));
}
