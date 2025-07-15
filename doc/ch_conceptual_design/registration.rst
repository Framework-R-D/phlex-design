Framework Registration
======================

Consider the following C++ classes and function:

.. code:: c++

   class hits { ... };
   class waveforms { ... };

   hits find_hits(waveforms const& ws) { ... }

where the implementations of :cpp:`waveforms`, :cpp:`hits`, and :cpp:`find_hits` are unspecified.
Suppose a physicist would like to use the function :cpp:`find_hits` to transform a data product labeled :cpp:`"Waveforms"` to one labeled :cpp:`"GoodHits"` for each spill with unlimited concurrency.
This can be achieved by in terms of the C++ *registration stanza*:

.. code-block:: c++
   :caption: Please work
   :name: register_transform

   PHLEX_REGISTER_ALGORITHMS()   // <== Registration opener (w/o configuration object)
   {
     products("GoodHits") =      // 1. Specification of output data product from find_hits
       transform(                // 2. Higher-order function
         "hit_finder",           // 3. Name assigned to HOF
         find_hits,              // 4. Algorithm/HOF operation
         concurrency::unlimited  // 5. Allowed CPU concurrency
       )
       .family(
         "Waveforms"_in("APA")   // 6. Specification of input data-product family (see text)
       );
   }

The registration stanza is included in a C++ file that is compiled into a :term:`module`, a compiled library that is dynamically loadable by Phlex.

A Python algorithm can be registered with its own companion C++ module or through the Python import helpers that make use of a pre-built, configurable, Phlex module.
For the sake of consistency and ease of understaning, the helpers have the same naming and follow the same conventions as the C++ registration.

The stanza is introduced by an *opener*—e.g. :cpp:`PHLEX_REGISTER_ALGORITHMS()`—followed by a *registration block*, a block of code between two curly braces that contains one or more *registration statements*.
A registration statement is a programming statement that closely follows the equation described in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions` and is used to register an algorithm with the framework.

.. math::

   \ifamily{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \ifamily{a}{\text{input}}

Specifically, in the registration stanza above, we have the following:

   :cpp:`products(...)`
     1. This is the equivalent of the output family :math:`\ifamily{b}{\text{output}}`, which is formed from specification(s) of the data product(s) created by the algorithm :need:`DUNE 156`.
        One of the fields of the data-product specification is the data layer to which the data products will belong :need:`DUNE 90`.
        Phlex does not require the output and input categories to be the same.

   :cpp:`transform(...)`
     Fully specifying the mathematical expression :math:`\text{HOF}(f_1,\ f_2,\ \dots)` requires several items:

     2. The HOF to be used,
     3. The name to assign to the configured HOF,
     4. The algorithm/HOF operator(s) to be used (i.e. :math:`f_1,\ f_2,\ \dots`), and
     5. The maximum number of CPU threads the framework can use when invoking the algorithm :need:`DUNE 152`.

   :cpp:`family(...)`
     6. The specification of the input family :math:`\ifamily{a}{\text{input}}` requires (a) the specification of data products that serve as input family elements :need:`DUNE 65`, and (b) the label of the data layer in which the input data products are found.

The set of information required by the framework for registering an algorithm largely depends on the HOF being used (see the :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions` for specific interface).
However, in general, the registration code will specify which data products are required/produced by the algorithm :need:`DUNE 111` and the hardware resources required by the algorithm :need:`DUNE 9`.
Note that the input and output data-product specifications are matched with the corresponding types of the registered algorithm's function signature.
In other words:

- :cpp:`"Waveforms"` specifies a data product whose C++ type is that of the first (and, in this case, only) input parameter to :cpp:`find_hits` (i.e. :cpp:`waveforms`).
- :cpp:`"GoodHits"` specifies a data product whose C++ type is the :cpp:`hits` return type of :cpp:`find_hits`.

When executed, the above code creates a :term:`configured higher-order function`, which serves as a node in the function-centric data-flow graph.

The registration block may contain any code supported by C++.
The block, however, must contain a registration statement to execute an algorithm.

.. important::

   A module must contain only one registration stanza.
   Note that multiple registration statements may be made in each stanza.

Algorithms with Multiple Input Data Products
--------------------------------------------

The registration example given above in :numref:`ch_conceptual_design/registration:Framework Registration` creates an output family by applying a one-parameter algorithm :cpp:`find_hits` to each element of the input family, as specified by :cpp:`family("Waveforms"_in("APA"))`.
In many cases, however, the algorithm will require more than one data product.
Consider another algorithm :cpp:`find_hits_subtract_pedestals`, which forms hits by first subtracting pedestal values from the waveforms, both of which are presented to the algorithm as data products from the `APA`.
The interface of the algorithm and its registration would look like:

.. code:: c++

  class hits { ... };
  class waveforms { ... };
  class pedestals { ... };
  hits find_hits_subtract_pedestals(waveforms const&, pedestals const&) {...}

  PHLEX_REGISTER_ALGORITHMS(config)
  {
    products("GoodHits") =
      transform("find_hits", find_hits_subtract_pedestals, concurrency::unlimited)
      .family("Waveforms"_in("APA"), "Pedestals"_in("APA"));
  }

The elements of the input family are thus pairs of the data products labeled :cpp:`"Waveforms"` and :cpp:`"Pedestals"` in each APA. [#zip]_
In this case, the data cell for both data products is the same—i.e. for a given invocation of :cpp:`find_hits_subtract_pedestals`, both data products will be associated with the same APA.

There are cases, however, where an algorithm needs to operate on data products from *different* data cells :need:`DUNE 89`.

.. note::

   The number of arguments presented to the :cpp:`family(...)` clause must match the number of input parameters to the registered algorithm.
   The order of the :cpp:`family(...)` arguments also corresponds to the order of the algorithm's input parameters.

Data Products from Different Data Categories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Consider the operator :math:`\textit{make\_vertices}` in :numref:`workflow` that requires two arguments: the :math:`\textit{GoodTracks}` collection for each APA (data layer `APA`), and the detector :math:`\textit{Geometry}` that applies for the entire job (data layer `Job`) [#job]_.
This would be expressed in C++ as:

.. code:: c++

   vertices make_vertices(tracks const&, geometry const&) { ... }

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("Vertices") =
       transform("vertex_maker", make_vertices, concurrency::unlimited)
       .family("GoodHits"_in("APA"), "Geometry"_in("Job"));
   }

where the categories are explicit in the family statement.

Phlex supports such uses cases :need:`DUNE 113`, even if the specified categories are unrelated to each other.
For example, suppose an algorithm needed to access a data product from a `Spill`, and it also required a calibration offset provided from an external database table :need:`DUNE 35`.
Instead of providing a separate mechanism for handling calibration constants, a separate layer could be invented (e.g. `Calibration`) whose data cells corresponded to intervals of validity.
So long as a relation can be defined between specific `Spill` data cells and specific `Calibration` data cells, the framework can use that relation to form the input family of `Spill`\ -\ `Calibration` data-product pairs that are presented to the algorithm.
How the relation between data cells is defined is referred to as *data marshaling*, and it is described further in :numref:`ch_subsystem_design/task_management:Data-Marshaling`.

Data Products from Adjacent Data Cells
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In some cases, it may be necessary to simultaneously access data products from adjacent data-products sets :need:`DUNE 91`, where *adjacency* is defined by the user :need:`DUNE 92`.
The notion of adjacency can be critical for (e.g.) time-windowed processing (see :numref:`ch_conceptual_design/hofs/windows:Windows`), where the details of the "next" time bin are needed to accurately calculate properties of the "current" time bin.

Supporting the processing of adjacent data cells is described further in :numref:`ch_subsystem_design/task_management:Data-Marshaling`.

Accessing Configuration Information
-----------------------------------

Instead of hard-coding all pieces of registration information, it is desirable to specify a subset of such information through a program's run-time configuration.
To do this, an additional argument (e.g. :cpp:`config`) is passed to the registration opener:

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     auto selected_data_layer = config.get<std::string>("data_layer");

     products("GoodHits") =
       transform("hit_finder", find_hits, concurrency::unlimited)
       .family("Waveforms"_in(selected_data_layer));
   }

.. note::

   As discussed in :numref:`ch_subsystem_design/configuration:Programmatic access to the configuration`, the registration code will have access only to the configuration relevant to the algorithm being registered, and to certain framework-level configuration such as debug level, verbosity, or parallelization options.

Except for the specification of :cpp:`find_hits` as the algorithm to be invoked, and :cpp:`transform` as the HOF, all other pieces of information may be provided through the configuration.

Framework Dependence in Registration Code
-----------------------------------------

Usually, classes like :cpp:`waveforms` and :cpp:`hits` and algorithms like :cpp:`find_hits` are framework-independent (see :numref:`introduction:Framework Independence`).
There may be scenarios, however, where dependence on framework interface is required, especially if framework-specific metadata types are used by the algorithm.
In such cases, it is strongly encouraged to keep framework dependence within the module itself and, more specifically, within the registration stanza.
This can be often achieved by registering closure objects that are generated by lambda expressions.

For example, suppose a physicist would like to create an algorithm :cpp:`find_hits_debug` that reports a spill number when making tracks.
By specifying a lambda expression that takes a :cpp:`phlex::handle<waveforms>` object, the data product can be passed to the :cpp:`find_hits_debug` function, along with the spill number from the metadata accessed from the handle:

.. code:: c++

   hits find_hits_debug(waveforms const& ws, std::size_t apa_number) { ... }

   PHLEX_REGISTER_ALGORITHMS(m)
   {
     products("GoodHits") =
       transform(
         "hit_finder",
         [](phlex::handle<waveforms> ws) { return find_hits_debug(*ws, ws.id().number()); },
         concurrency::unlimited
       )
       .family("Waveforms"_in("APA"));
   }

The lambda expression *does* depend on framework interface; the :cpp:`find_hits_debug` function, however, retains its framework independence.

Member Functions of Classes
---------------------------

In some cases, it may be necessary to register a class and its member functions with the framework.
This is done by first creating an instance of the class by invoking :cpp:`make<T>(args...)`, where :cpp:`T` is the user-defined type, and :cpp:`args...` are the arguments presented to :cpp:`T`'s constructor.
For example, the :cpp:`find_hits` algorithm author could have instead created a :cpp:`hit_finder` class, whose constructor takes a parameter called :cpp:`sigma_threshold`:

.. code:: c++

   class hit_finder {
   public:
     hit_finder(float sigma_threshold);
     hits find(waveforms const& ws) const;
     ...
   };

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     auto sigma_threshold = config.get<float>("sigma_threshold");
     auto selected_data_layer = config.get<std::string>("data_layer");

     products("GoodHits") =
       make<hit_finder>(sigma_threshold)  // <= Make framework-owned instance of hit_finder
         .transform("hit_finder", &hit_finder::find, concurrency::unlimited)
         .family("Waveforms"_in(selected_data_scope));
   }

Note that the :cpp:`hit_finder` instance created in the code above is *owned by the framework*.
The :cpp:`hit_finder::find` member function's address is registered in the :cpp:`transform(...)` clause, thus instructing the framework to invoke
:cpp:`find`, bound to the framework-owned :cpp:`hit_finder` instance.

.. note::

  Algorithm authors should first attempt to implement algorithms as free functions.
  Registering class instances and their member functions with the framework should only be considered when:

  - multiple processing steps must work together, relying on shared internal data, or
  - supporting legacy code that relies on object-oriented design.

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

.. [#zip] The operation that forms the family :math:`\left[(\textit{Waveforms}_i, \textit{Pedestals}_i)\right]_{i \in \iset{\text{APA}}}` from the separate families :math:`\ifamily{\textit{Waveforms}}{\text{APA}}` and :math:`\ifamily{\textit{Pedestals}}{\text{APA}}` is called *zip*.
.. [#job] As shown in :numref:`data-organization`, there is a `Job` data layer, to which job-level data products may belong.
.. [#f1] Equivalently, one can use the obscure syntax :cpp:`transform(..., static_cast<double(*)(double)>(std::sqrt), ...)`, where :cpp:`std::sqrt` is cast to the desired overload.
