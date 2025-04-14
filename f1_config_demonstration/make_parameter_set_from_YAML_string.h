#ifndef MAKE_PARAMETER_SET_FROM_YAML_STRING_H
#define MAKE_PARAMETER_SET_FROM_YAML_STRING_H
#include "fhiclcpp/ParameterSet.h"

#include <string>

fhicl::ParameterSet make_parameter_set_from_YAML_string(std::string const &yaml_in);

#endif /* MAKE_PARAMETER_SET_FROM_YAML_STRING_H */

// Local Variables:
// mode: c++
// End:
