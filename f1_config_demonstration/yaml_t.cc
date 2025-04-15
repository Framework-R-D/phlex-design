#include "fhiclcpp/ParameterSet.h"

#include "make_parameter_set_from_YAML.h"

#include <iostream>
#include <string>

int main()
{
  auto pset = make_parameter_set_from_YAML_file("YAML.yaml");
  std::cout << pset.to_indented_string(0, true) << std::endl;
}
