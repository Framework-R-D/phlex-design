#include "fhiclcpp/ParameterSet.h"

#include "libjsonnet++.h"
#include "make_parameter_set_from_YAML_string.h"

#include <iostream>
#include <string>

int main()
{
  jsonnet::Jsonnet json_reader;
  json_reader.init();
  std::string json_output;
  auto result [[maybe_unused]] = json_reader.evaluateFile("JSON.json", &json_output);
  if (!result) {
    std::cerr << json_reader.lastError();
    exit(1);
  }
  // Legal JSON is also legal YAML.
  auto pset = make_parameter_set_from_YAML_string(json_output);
  std::cout << pset.to_indented_string(0, true) << std::endl;
}
