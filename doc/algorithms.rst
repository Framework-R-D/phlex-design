Algorithms
==========

As :ref:`mentioned earlier <programming_paradigm:Higher-order functions supported by Phlex>`, an algorithm is registered with the framework as an operator to a higher-order function (HOF).
The specific signature expected of each algorithm depends on which HOF is needed (see the :ref:`hof_operators:HOF operators` below).
Usually, an algorithm does not need to depend on framework interface :dune:`20 Algorithms independent of framework interface`.
There may be scenarios, however, where dependence on framework interface is required, especially if framework-specific metadata types are used by the algorithm.

Allowed signatures for registered algorithms
--------------------------------------------

In general, Phlex supports the registration of C++ algorithms with function signatures like (see :ref:`below <algorithms:HOF operators>`):

.. code:: c++

   return_type function_name(P1, ..., Pn, R1, ..., Rm) [qualifiers];

where the types :cpp:`P1, ..., Pn` denote types of data products and the types :cpp:`R1, ..., Rm` indicate resources.
We will first discuss the :ref:`data-product and resource types <algorithms:Input parameters>`, followed by the :ref:`return type <algorithms:Return types>`, and then the :ref:`function name and optional qualifers <algorithms:Function names and qualifiers>`.

Input parameters
^^^^^^^^^^^^^^^^

A data product of type :cpp:`P` can be presented to a C++ algorithm if the corresponding input parameter (i.e. the relevant :cpp:`T` types) is one of the following:

- :cpp:`P const&` — immutable access to a data product provided through a reference
- :cpp:`P const*` — immutable access to a data product provided through a pointer
- :cpp:`P const` — the data product is copied into an immutable object (assumes data product is copyable)
- :cpp:`P` — the data product is copied into a mutable object (assumes data product is copyable)
- :cpp:`phlex::handle<P>` — a lightweight object that provides immutable access to a data product as well as any metadata associated with it

For each of these cases, the data product itself remains immutable.

Whereas data-product types are allowed to be copyable and immutable, resources of type :cpp:`R` are not assumed to be copyable or immutable.
The following types are therefore supported:

- :cpp:`R const&` — immutable access to a resource provided through a reference
- :cpp:`R const*` — immutable access to a resource provided through a pointer
- :cpp:`R&` — mutable access to a resource provided through a reference (if mutable access permitted by resource)
- :cpp:`R*` — mutable access to a resource provided through a pointer (if mutable access permitted by resource)
- :cpp:`phlex::resource<R>` — a lightweight object that provides access to the resource as well as any metadata associated with it

Resources are described in more detail :ref:`here <resources:Resources>` along with a motivation for why mutable access is sometimes necessary

Return types
^^^^^^^^^^^^

The meaning of an algorithm's return type depends on the HOF and is discussed in the :ref:`section on HOF operators <hof_operators:HOF operators>`.
However, to simplify the discussion we introduce to concept of the *created data-product type*.
For Phlex to appropriately schedule the execution of algorithms and manage the lifetimes of data products, the framework itself must retain ownership of the data products.
This means that the data products created by algorithms must have types that connote unique ownership.
Such semantics are conveyed to the framework by returning *values* and not null objects.

A created data-product type supports either:

- :cpp:`T`, where :cpp:`T` is not a pointer or reference type
- :cpp:`std::unique_ptr<T>`, where the created object is non-null

The following types (or their equivalents) are forbidden as created data-product types because they do not imply unambiguous ownership:

- *bare pointer types*, such as :cpp:`T*` or :cpp:`T const*`
- *reference types*, such as :cpp:`T&` or :cpp:`T const&`

Function names and qualifiers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:`function_name` :ref:`above <algorithms:Allowed signatures for registered algorithms>` may be any function name supported by the C++ language.
Code authors should aim to implement algorithms as free functions.
However, in some cases it may be necessary for class member functions to be used instead.
When member functions are required, the qualifier :cpp:`const` should be specified to indicate that the class instance remains immutable during the execution of the member function [#f1]_.

Framework registration
----------------------

.. important::

   The C++ interface below is illustrative and not intended to reflect the final registration interface.

Consider the following C++ classes and function:

.. code:: c++

   class hits { ... };
   class tracks { ... };

   tracks make_tracks(hits const& hs) { ... }

where the implementations of `hits`, `tracks`, and `make_tracks` are unspecified.
Phlex adopts a `fluent interface <https://en.wikipedia.org/wiki/Fluent_interface>`_ for registering algorithms.
This makes it possible to express the sentence:

    *With the function* `make_tracks` *and unlimited concurrency, transform "hits" to "tracks" for each event.*

in terms of the C++ code:

.. code:: c++

   ALGORITHMS(m)
   {
     m.with(
            make_tracks,           // (1) Algorithm/HOF operator
            concurrency::unlimited // (2) Allowed CPU concurrency
           )
      .transform(                  // (3) Higher-order function
                 "hits"            // (4) Input data product
                )
      .to("tracks")                // (5) Output data product
      .for_each("event");          // (6) Where to find input data product/place output data product
   }

The above code specifies 6 pieces of information:

1. The algorithm/HOF operator to be used
2. The maximum number of CPU threads the framework can use when invoking the algorithm :dune:`4.1 Algorithms can use multiple CPUs`
3. The HOF to be used (generally expressed as an active verb)
4. The product name(s) from which to form the input data product sequence
5. The name(s) of the data product created by the algorithm
6. The data category where the input data products are found and the output data products are to be placed

The set of information required by the framework for registering an algorithm largely depends on the HOF being used (see the :ref:`section on HOF operators <hof_operators:HOF operators>` for specific interface).
However, in general, the registration code will specify which data products are required/produced by the algorithm :dune:`1.1 Algorithm Communication Via Data Products` and the hardware resources required by the algorithm :dune:`4 Algorithm hardware requirements`.

When executed, the above code creates a :term:`configured higher-order function <Configured higher-order function (CHOF)>`, which serves as a node in the function-centric data-flow graph.

Accessing configuration information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A more typical usage pattern is to access information from the program's configuration.

.. code:: c++

   ALGORITHMS(m, config)
   {
     auto selected_data_category = config.get<std::string>("data_category", "event");
     m.with(make_tracks, concurrency::unlimited)
      .transform("hits")
      .to("tracks")
      .for_each(selected_data_category);
   }


Framework dependence in registration code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note that the classes `hits` and `tracks` and the function `make_tracks` do not depend on any framework interface.

Lambda expressions
^^^^^^^^^^^^^^^^^^

Lambda expressions may be preferable when needing to register overloaded functions.

Member functions of classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: c++

   class track_maker {
   public:
     track_maker(std::size_t track_seed);
     tracks make(hits const& hs) const;
     ...
   };

   ALGORITHMS(m, config)
   {
     auto track_seed = config.get<std::size_t>("track_seed");
     auto selected_data_category = config.get<std::string>("data_category", "event");

     m.make<track_maker>(track_seed)
      .with(&track_maker::make, concurrency::unlimited)
      .transform("hits")
      .to("tracks")
      .for_each(selected_data_category);
   }

HOF operators
^^^^^^^^^^^^^

The specific function signature of the algorithm depends on the HOF to which the algorithm serves as an operator.
Please see the below links for details on each operator.

.. toctree::
   :maxdepth: 2

   hof_operators

.. rubric:: Footnotes

.. [#f1] Phlex permits the registration of member functions that do not use the :cpp:`const` qualifier.
         However, using such functions is highly discouraged as it indicates a class instance is modifiable during member-function execution, which is at odds with Phlex's functional-programming paradigm.
