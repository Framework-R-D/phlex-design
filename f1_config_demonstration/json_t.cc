// #include "fhiclcpp/ParameterSet.h"
// #include "cetlib/filepath_maker.h"

#include "libjsonnet++.h"

#include <iostream>
#include <string>

int main()
{
  jsonnet::Jsonnet json_reader;
  json_reader.init();
  std::string json_output;
  auto result [[maybe_unused]]  = json_reader.evaluateFile("JSON.json", &json_output);
  if (!result) {
    std::cerr << json_reader.lastError();
    exit(1);
  }
  std::cout << json_output << std::endl;
  // cet::filepath_maker maker{};
  // auto pset = fhicl::ParameterSet::make("FHiCL.fcl", maker);
  // std::cout << pset.to_indented_string(0, true) << std::endl;
}
