---
author: "Chris Green"
date: "2024-06-05"
---

# Design Ideas for the DUNE Framework

## Data

* "Data products" and "data groups" are named, unordered, heterogeneous,
  associative collections of "concrete data."

* "Concrete data" are specific instances of:

  * basic data types

  * "data groups"

  * "views"

  * "sequences"

* All data products and their components shall be _persistable_.

* The in-memory representations of data products or data groups with a
  specific name shall be specified by exactly one data description per
  name in an appropriate format.

* Named components of a data product or data group may be flagged as
  transient and therefore not persisted by default.

* Persistence of any given instance of a data product shall be
  determined by the scheduler with input from the applicable DDL, user
  configuration, and from algorithm-specific metadata.

* A data product is the smallest unit of user data whose provenance is
  tracked by the framework. It _may not_ be incorporated as concrete
  data into another data product, but it _may_ be incorporated into
  another data product via a view.

* A data product with a specific name must be associated with exactly
  one data family.

* A given instance of a data product may have metadata associated with
  it via a single named data group.

* A "sequence" is an ordered list of values of a single specified
  concrete data type.

* A "view" is exactly one of:

  * An unique identifier of a single data product.

  * An unique identifier of a data group, or sequence in the same or a
    different data product.

  * An unique identifier of a member of the containing data group, or a
    different data group in the same or a different data product.

  * An unique identifier of a sequence in the same or a different data
    product.

  * An ordered list of slices of a single uniquely-identified sequence
    in the same or a different data product. If the sequence members are
    of an associative type, the view may specify exactly one name.

  * A bi-directional association between two views, with an optional
    data group (or view thereto).

  The in-memory representation of a view may be transparent
  (fully-resolved), or not (views must be deferenced by the algorithm)
  as specified in the defintion of a data product in order to provide
  contiguous data for memory-efficient access.

* The persistent representation of a specific data product or group is
  determined by the I/O-specific backend plugin with input from the
  applicable DDL, the scheduler, and user configuration.

## Algorithms

* An "algorithm" is executable object code, visible to the framework as
  a single entity callable via Foreign Function interface (FFI).

* An algorithm's data dependencies must be calculable at initialization
  time, and static thereafter.

* Algorithm execution overhead must be As Low As Reasonably Achievable
  (ALARA).

* All algorithm arguments shall be input arguments only, and shall
  consist solely of a single argument representing the configuration,
  and zero or more "data arguments."

* A "data argument" represents exactly one of:

  * A single instance of a data product.

  * A single instance of data product along with its associated metadata
    data group.

  * A single instance of a data group representing the metadata
    associated with the current data set of a named data family.

* All output from an algorithm shall be communicated via return value as
  allowed by the FFI specification, and consist of one or more data
  products along with their (non-optional) associated metadata.

* An Algorithm shall be made known to the framework via a registration
  operation specifying the data products required and produced, resource
  requirements, and other requirements of the algorithm.

* Algorithms may be registered with the framework multiple times; each
  registered instance shall have an unique identifier that can be
  referenced in user configuration.

* Each algorithm registration operation will be provided with (and may
  utilize) appropriate user configuration.

## Data families

* A Data family shall be made known to the framework via a registration
  operation specifying its name, any hiearchical relationship with other
  data families, and providing details of functions that can be called
  by the framework to:
  
  * Initialize and identify a new data set in that family.
  
  * Provide the identity of the "current" data set of that family (if it
    exists).

  * Provide the identity of a data set associated with a specific data
    product, or a suitable "adjacent" data set.
