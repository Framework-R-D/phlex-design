# Why Are Framework Agnostic Algorithms Problematic?

## Givens and assumptions

### Givens

1. The framework must be capable of executing framework-agnostic
   algorithms i.e. algorithms that do not depend upon framework
   libraries, C++ modules or headers.

1. The algorithm registration code _is_ allowed to depend on framework
   headers.

1. The algorithm (when considered as a black box) is functional in
   nature: any data exchange with other algorithms and/or the outside
   world must take place solely via the input arguments and returned
   data in order to satisfy requirements for reproducibility, and for
   verifiability via provenance.

1. An algorithm must be configurable by the framework: in other words,
   if the configuration method is not dependent on the framework, then
   the framework must be provided with an algorithm-specific way of
   converting its own representation of the algorithm's configuration
   into one the algorithm can understand.

1. Users are people, and will do the mininum (including learning new
   things) to have something that works for what they need.

### Assumptions (negotiable)

1. Data passed into and retrieved from the algorithm need not be
   framework-independent (though if they must be such, I would claim
   that only strengthens my argument).

## Walking through the reasoning

### The likely signature of an algorithm

```.c++
output_data_or_tuple_type
reco_algorithm(algo_config_type const & config,
               input_data_type_A const & input_A1,
               input_data_type_A const & input_A2,
               input_data_type_C const & input_C1,
               ...);
```

### Thoughts and consequences

1. The user must be able to specify exactly what input data are required
   where necessary: applicable parameters, data family and/or dataset
   characteristcs and relationships.

1. Also, the wider context of the output data must be clearly specified
   to the framework in a way that allows the framework to satisfy the
   input data requirements of other algorithms and/or the user-specified
   composition of the output files.

1. These specifications must be received and understandable by the
   framework; in the context of our givens, that means that the
   information must be conveyed to the framework either via static
   configuration, or via the registration code.

1. In particular, communication with external databases (e.g. for
   calibration data), must be carefully specified and the data must be
   provided by said external database in a way both that the algorithm
   can understand, **and** such that the framework can negotiate with
   the external resource for (and/or cache) the data and ensure its
   reproducibility if required (e.g. by writing it to an output file).

## Main thesis

1. Any framework independence of the algorithm function itself is not
   particularly useful if a significant part of the operation of the
   algorithm is dependent upon the registration code, either due to
   composition, or data specification, or other factors. This would
   simply require more and more, "non-framework framework code" to
   emulate enough of the registration code and what the framework does
   and/or provides as a result.
   
1. The main argument I've been hearing about the need for
   framework-agnostic algorithms is the need for easy testing of
   algorithm code, although it's likely that some people actually want
   to use their algorithms in a non-framework context for physics
   analysis. In either case though, it seems to me as if the need for
   significant non-framework framework code is sufficient to frustrate
   both aims. Either that, or the need to minimize that code necessarily
   limits what the framework is able to provide.
