#include "fhiclcpp/ParameterSet.h"
#include "cetlib/filepath_maker.h"

#include <iostream>

int main()
{
  cet::filepath_maker maker{};
  auto pset = fhicl::ParameterSet::make("FHiCL.fcl", maker);
  std::cout << pset.to_indented_string(0, true) << std::endl;
}
