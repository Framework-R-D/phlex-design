Framework Registration
======================

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

A Python algorithm can be registered with its own companion C++ module or through the Python import helpers that make use of a pre-built, configurable, Phlex module.
For the sake of consistency and ease of understaning, the helpers have the same naming and follow the same conventions as the C++ registration.

The stanza is introduced by an *opener*—e.g. :cpp:`PHLEX_REGISTER_ALGORITHMS()`—followed by a *registration block*, a block of code between two curly braces that contains one or more *registration statements*.
A registration statement is a programming statement that closely follows the equation described in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions` and is used to register an algorithm with the framework.

.. math::

   \isequence{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \isequence{a}{\text{input}}

Specifically, in the registration stanza above, we have the following:

   :cpp:`products(...)`
     1. This is the equivalent of the output sequence :math:`\isequence{b}{\text{output}}`, which is formed from specification(s) of the data product(s) created by the algorithm :need:`DUNE 156`.
        One of the fields of the data-product specification is the data-product set category to which the data products will belong :need:`DUNE 90`.
        Phlex does not require the output and input categories to be the same.

   :cpp:`transform(...)`
     Fully specifying the mathematical expression :math:`\text{HOF}(f_1,\ f_2,\ \dots)` requires several items:

     2. The HOF to be used,
     3. The name to assign to the configured HOF,
     4. The algorithm/HOF operator(s) to be used (i.e. :math:`f_1,\ f_2,\ \dots`), and
     5. The maximum number of CPU threads the framework can use when invoking the algorithm :need:`DUNE 152`.

   :cpp:`sequence(...)`
     The specification of the input sequence :math:`\isequence{a}{\text{input}}` requires:

     6. The specification(s) of data products that serve as input sequence elements :need:`DUNE 65`.
     7. The data category where the input data products are found.

The set of information required by the framework for registering an algorithm largely depends on the HOF being used (see the :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions` for specific interface).
However, in general, the registration code will specify which data products are required/produced by the algorithm :need:`DUNE 111` and the hardware resources required by the algorithm :need:`DUNE 9`.
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

Algorithms with Multiple Input Data Products
--------------------------------------------

The registration example given above in :numref:`ch_conceptual_design/registration:Framework Registration` creates an output sequence by applying a one-parameter algorithm :cpp:`make_tracks` to each element of the input sequence, as specified by :cpp:`sequence("good_hits"_in("spill"))`.
In many cases, however, the algorithm will require more than one data product.
Suppose another algorithm :cpp:`make_tracks_loose` can form tracks from both the :cpp:`"good_hits"` data product and the :cpp:`"bad_hits"` data product for each spill.
The interface of the algorithm and its registration would look like:

.. code:: c++

  tracks make_tracks_loose(hits const& good, hits const& bad) {...}

  PHLEX_REGISTER_ALGORITHMS(m, config)
  {
    products("loose_tracks") =
      transform("loose_track_maker", make_tracks_loose, concurrency::unlimited)
      .sequence("good_hits"_in("spill"), "bad_hits"_in("spill"));
  }

The elements of the input sequence are thus pairs of the data products labeled :cpp:`"good_hits"` and :cpp:`"bad_hits"` in each spill. [#zip]_
In this case, the data-product set for both data products is the same—i.e. for a given invocation of :cpp:`make_tracks_loose`, both data products will be associated with the same spill.

There are cases, however, where an algorithm needs to operate on data products from *different* data-product sets :need:`DUNE 89`.

Data Products from Different Data Categories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Consider an algorithm :cpp:`make_vertices` that requires two arguments: the :cpp:`"good_tracks"` collection for each spill (data category :cpp:`"spill"`), and the detector :cpp:`"geometry"` that applies for the entire job (data category :cpp:`"job"`) [#job]_.
This would be expressed in C++ as:

.. code:: c++

   vertices make_vertices(tracks const&, geometry const&) { ... }

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     products("good_vertices") =
       transform("vertex_maker", make_vertices, concurrency::unlimited)
       .sequence("good_hits"_in("spill"), "geometry"_in("job"));
   }

where the categories are explicit in the sequence statement.

Phlex supports such uses cases :need:`DUNE 113`, even if the specified categories are unrelated to each other.
For example, suppose an algorithm needed to access a data product from a spill, and it also required a calibration offset provided from an external database table :need:`DUNE 35`.
Instead of providing a separate mechanism for handling calibration constants, a separate category could be invented (e.g. :cpp:`"calibration"`) whose data-product sets corresponded to intervals of validity.
So long as a relation can be defined between a specific :cpp:`"spill"` data-product sets and a specific :cpp:`"calibration"` data-product sets, the framework can use that relation to form the input sequence of spill-calibration pairs that are presented to the algorithm.
How the relation between data-product sets is defined is referred to as *data marshaling*, and it is described further in :numref:`ch_subsystem_design/task_management:Data-Marshaling`.

Data Products from Adjacent Data-Product Sets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In some cases, it may be necessary to simultaneously access data products from adjacent data-products sets :need:`DUNE 91`, where *adjacency* is defined by the user :need:`DUNE 92`.
The notion of adjacency can be critical for (e.g.) time-windowed processing (see :numref:`ch_conceptual_design/hofs/windows:Windows`), where the details of the "next" time bin are needed to accurately calculate properties of the "current" time bin.

Supporting the processing of adjacent data-product sets is described further in :numref:`ch_subsystem_design/task_management:Data-Marshaling`.

Accessing Configuration Information
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

   As discussed in :numref:`ch_conceptual_design/user_configuration:Programmatic access to the configuration`, the registration code will have access only to the configuration relevant to the algorithm being registered, and to certain framework-level configuration such as debug level, verbosity, or parallelization options.

Except for the specification of :cpp:`make_tracks` as the algorithm to be invoked, and :cpp:`transform` as the HOF, all other pieces of information may be provided through the configuration.

Framework Dependence in Registration Code
-----------------------------------------

Usually, classes like :cpp:`hits` and :cpp:`tracks` and algorithms like :cpp:`make_tracks` are framework-independent (see :numref:`introduction:Framework Independence`).
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

Member Functions of Classes
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

Overloaded Functions
--------------------

Phlex performs a substantial amount of type deduction through the :cpp:`transform(...)` clause.
This works well except in cases where the registered algorithms are overloaded functions.
For example, suppose one wants to register C++'s overloaded :cpp:`std::sqrt(...)` function with the framework.
Simply specifying :cpp:`transform(..., std::sqrt)` will fail at compile time as the compiler will not be able to determine which overload is desired.

Instead, the code author can use the following [#f1]_:

.. code:: c++

   transform(..., [](double x){ return std::sqrt(x); }, ...);

where the desired overload is selected based on the :cpp:`double` argument to the lambda expression.

.. rubric:: Footnotes

.. [#zip] The operation that forms the sequence :math:`\isequence{(\texttt{"good\_hits"}, \texttt{"bad\_hits"})}{\text{spill}}` from the separate sequences :math:`\isequence{\texttt{"good\_hits"}}{\text{spill}}` and :math:`\isequence{\texttt{"bad\_hits"}}{\text{spill}}` is called *zip*.
.. [#job] As shown in :numref:`data-organization`, there is a "Job" data category , to which job-level data products may belong.
.. [#f1] Equivalently, one can use the obscure syntax :cpp:`transform(..., static_cast<double(*)(double)>(std::sqrt), ...)`, where :cpp:`std::sqrt` is cast to the desired overload.
