Framework registration
======================

.. attention::

   The C++ interface below is illustrative and not intended to reflect the final registration interface.

To the extent possible, Phlex preserves data flow among data products and algorithms.
This is indicated in the interface for registering algorithms.
In some cases, access to a limited resource is required and the algorithm signature will specify dependencies on not only the data of interest but also the shared resource.

Consider the following C++ classes and function:

.. code:: c++

   class hits { ... };
   class tracks { ... };

   tracks make_tracks(hits const& hs) { ... }

where the implementations of :cpp:`hits`, :cpp:`tracks`, and :cpp:`make_tracks` are unspecified.
Suppose a physicist would like to use the function :cpp:`make_tracks` to transform "good_hits" to "good_tracks" for each spill with unlimited concurrency.
This can be achieved by in terms of the C++ *registration stanza*:

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m)  // <== Registration opener (w/o configuration object)
   {
     products("good_tracks") =   // 1. Specification of output data product from make_tracks
       transform(                // 2. Higher-order function
         "track_maker",          // 3. Name assigned to HOF
         make_tracks,            // 4. Algorithm/HOF operation
         concurrency::unlimited  // 5. Allowed CPU concurrency
       )
       .sequence(
         "good_hits"             // 6. Specification of input data product to make_tracks
         _in("spill")            // 7. Data category to search for input data products
       );
   }

The registration stanza is included in a C++ file that is compiled into a :term:`module`, a compiled library that is dynamically loadable by Phlex.

The stanza is introduced by an *opener*—e.g. :cpp:`PHLEX_REGISTER_ALGORITHMS()`—followed by a *registration block*, a block of code between two curly braces that contains one or more *registration statements*.
A registration statement is a programming statement that closely follows the equation described in :numref:`functional_programming:Higher-order functions supported by Phlex`:

.. math::

   \sequence{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \sequence{a}{\text{input}}

Specifically, in the registration stanza above, we have the following:

   :cpp:`products(...)`
     1. This is the equivalent of the output sequence :math:`\sequence{b}{\text{output}}`, which is formed from specification(s) of the data product(s) created by the algorithm :dune:`29 Specification of data products created by an algorithm`.

   :cpp:`transform(...)`
     Fully specifying the mathematical expression :math:`\text{HOF}(f_1,\ f_2,\ \dots)` requires several items:

     2. The HOF to be used,
     3. The name to assign to the configured HOF,
     4. The algorithm/HOF operator(s) to be used (i.e. :math:`f_1,\ f_2,\ \dots`), and
     5. The maximum number of CPU threads the framework can use when invoking the algorithm :dune:`24.2 Specification of algorithm's maximum number of CPU threads`.

   :cpp:`sequence(...)`
     The specification of the input sequence :math:`\sequence{a}{\text{input}}` requires:

     6. The specification(s) of data products that serve as input sequence elements :dune:`28 Specification of data products required by an algorithm`, and
     7. The data category where the input data products are found.

The set of information required by the framework for registering an algorithm largely depends on the HOF being used (see the :numref:`registration:HOF operators` for specific interface).
However, in general, the registration code will specify which data products are required/produced by the algorithm :dune:`1.1 Algorithm Communication Via Data Products` and the hardware resources required by the algorithm :dune:`4 Algorithm hardware requirements`.
Note that the input and output data-product specifications are matched with the corresponding types of the registered algorithm's function signature.
In other words:

- :cpp:`"good_hits"` specifies a data product whose C++ type is that of the first (and, in this case, only) input parameter to :cpp:`make_tracks` (i.e. :cpp:`hits`).
- :cpp:`"good_tracks"` specifies a data product whose C++ type is the :cpp:`tracks` return type of :cpp:`make_tracks`.

When executed, the above code creates a :term:`configured higher-order function <Configured higher-order function (CHOF)>`, which serves as a node in the function-centric data-flow graph.

The registration block may contain any code supported by C++.
The block, however, must contain a registration statement to execute an algorithm.

.. important::

   A module must contain only one registration stanza.
   Note that multiple registration statements may be made in each stanza.

Accessing configuration information
-----------------------------------

Instead of hard-coding all pieces of registration information, it is desirable to specify a subset of such information through a program's run-time configuration.
To do this, an additional argument (e.g. :cpp:`config`) is passed to the registration opener:

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto selected_data_scope = config.get<std::string>("data_scope");

     products("good_tracks") =
       transform("track_maker", make_tracks, concurrency::unlimited)
       .sequence("good_hits"_in(selected_data_scope));
   }

.. note::

   As discussed in :numref:`user_configuration:Programmatic access to the configuration`, the registration code will have access only to the configuration relevant to the algorithm being registered, and to certain framework-level configuration such as debug level, verbosity, or parallelization options.

Except for the specification of :cpp:`make_tracks` as the algorithm to be invoked, and :cpp:`transform` as the HOF, all other pieces of information may be provided through the configuration.

Framework dependence in registration code
-----------------------------------------

Usually, classes like :cpp:`hits` and :cpp:`tracks` and algorithms like :cpp:`make_tracks` do not need to depend on framework interface :dune:`20 Algorithms independent of framework interface`.
There may be scenarios, however, where dependence on framework interface is required, especially if framework-specific metadata types are used by the algorithm.
In such cases, it is strongly encouraged to keep framework dependence within the module itself and, more specifically, within the registration stanza.
This can be often achieved by registering closure objects that are generated by lambda expressions.

For example, suppose a physicist would like to create an algorithm :cpp:`make_tracks_debug` that reports a spill number when making tracks.
By specifying a lambda expression that takes a :cpp:`phlex::handle<hits>` object, the data product can be passed to the :cpp:`make_tracks_debug` function, along with the spill number from the metadata accessed from the handle:

.. code:: c++

   tracks make_tracks_debug(hits const& hs, std::size_t spill_number) { ... }

   PHLEX_REGISTER_ALGORITHMS(m)
   {
     products("good_tracks") =
       transform(
         "track_maker",
         [](phlex::handle<hits> hs) {
           return make_tracks_debug(*hs, hs.id()->number());
         },
         concurrency::unlimited
       )
       .sequence("good_hits"_in("spill"));
   }

The lambda expression *does* depend on framework interface; the :cpp:`make_tracks_debug` function, however, retains its framework independence.

.. todo::

   Further explain what framework independence means.
   For example, if one passes a framework-dependent closure to an algorithm, is that algorithm still framework-independent?

Member functions of classes
---------------------------

.. code:: c++

   class track_maker {
   public:
     track_maker(std::size_t track_seed);
     tracks make(hits const& hs) const;
     ...
   };

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto track_seed = config.get<std::size_t>("track_seed");
     auto selected_data_scope = config.get<std::string>("data_scope");

     products("good_tracks") =
       m.make<track_maker>(track_seed)
         .transform("track_maker", &track_maker::make, concurrency::unlimited)
         .sequence("good_hits"_in(selected_data_scope));
   }

Overloaded functions
--------------------

Phlex performs a substantial amount of type deduction through the :cpp:`transform(...)` clause.
This works well except in cases where the registered algorithms are overloaded functions.
For example, suppose one wants to register C++'s overloaded :cpp:`std::sqrt(...)` function with the framework.
Simply specifying :cpp:`transform(..., std::sqrt)` will fail at compile time as the compiler will not be able to determine which overload is desired.

Instead, the code author can use the following [#f1]_:

.. code:: c++

   transform(..., [](double x){ return std::sqrt(x); }, ...);

where the desired overload is selected based on the :cpp:`double` argument to the lambda expression.

.. include:: hof_operators.rst

.. rubric:: Footnotes

.. [#f1] Equivalently, one can use the obscure syntax :cpp:`transform(..., static_cast<double(*)(double)>(std::sqrt), ...)`, where :cpp:`std::sqrt` is cast to the desired overload.
