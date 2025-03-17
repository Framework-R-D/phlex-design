Algorithms
==========

As :ref:`mentioned earlier <programming_paradigm:Higher-order functions supported by Phlex>`, an algorithm is registered with the framework as an operator to a higher-order function (HOF).
The specific signature expected of each algorithm depends on which HOF is needed (see the :ref:`algorithms:HOF operators` below).
Usually, an algorithm does not need to depend on framework interface :dune:`20 Algorithms independent of framework interface`.
There may be scenarios, however, where dependence on framework interface is required, especially if framework-specific metadata types are used by the algorithm.

Framework registration
----------------------

.. important::

   The C++ interface below is illustrative and not intended to reflect the final registration interface.

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

The set of information required by the framework for registering an algorithm largely depends on the HOF being used (see :ref:`below <algorithms:HOF operators>` for specific interface).
However, in general, the registration code will specify which data products are required/produced by the algorithm :dune:`1.1 Algorithm Communication Via Data Products` and the hardware resources required by the algorithm :dune:`4 Algorithm hardware requirements`.

When executed, the above code creates a :term:`configured higher-order function <Configured higher-order function (CHOF)>`, which serves as a node in the function-centric data-flow graph.

Accessing configuation information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

Lambda expressions
^^^^^^^^^^^^^^^^^^

Lambda expressions may be preferable when needing to register overloaded functions.

Member functions of classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Multiple input arguments
^^^^^^^^^^^^^^^^^^^^^^^^

Multiple output arguments
^^^^^^^^^^^^^^^^^^^^^^^^^

Allowed types in function signatures
------------------------------------



HOF operators
-------------

Transforms
^^^^^^^^^^

+----------------------------+--------------------------------------------------+-----------------------------------------------+
| Transform                                                                     | Resulting sequence                            |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                   | Sequence transformation                          | Dimension                 | Length            |
+============================+==================================================+===========================+===================+
| :math:`f: A \rightarrow B` | .. math::                                        | :math:`\dim(b) = \dim(a)` | :math:`|b| = |a|` |
|                            |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                           |                   |
|                            |    \underbrace{(b_{i_1\dots i_n})}_b             |                           |                   |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+

Filters and predicates
^^^^^^^^^^^^^^^^^^^^^^

+--------------------------------------------------------------------------------------------+---------------------------------------------------+
| Filter                                                                                     | Resulting sequence                                |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| Operator (predicate)                    | Sequence transformation                          | Dimension                  | Length               |
+=========================================+==================================================+============================+======================+
| :math:`p: A \rightarrow \mbox{Boolean}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| \le |a|` |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_{a'}          |                            |                      |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+

Phlex will not schedule a predicate for execution if it is not bound to a filter.
Phlex will also not schedule a filter for execution if there are no non-filter algorithms downstream of it.

Observers
^^^^^^^^^

+----------------------------------------------------------------------------------------+-----------------------------------------------+
| Observer                                                                               | Resulting sequence                            |
+-------------------------------------+--------------------------------------------------+----------------------------+------------------+
| Operator                            | Sequence transformation                          | Dimension                  | Length           |
+=====================================+==================================================+============================+==================+
| :math:`p: A \rightarrow \mathbb{1}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| = 0` |
|                                     |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                  |
|                                     |    \underbrace{(\quad)}_{a'}                     |                            |                  |
+-------------------------------------+--------------------------------------------------+----------------------------+------------------+

As :ref:`mentioned earlier <programming_paradigm:Higher-order functions supported by Phlex>`, observers are a special case of filters that always reject the data presented to them.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Folds
^^^^^

+----------------------------------------------------------------------------------------+-----------------------------------------------+
| Fold                                                                                   | Resulting sequence                            |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                            | Sequence transformation                          | Dimension                 | Length            |
+=====================================+==================================================+===========================+===================+
| :math:`g: C \times D \rightarrow D` | .. math::                                        | :math:`\dim(d) < \dim(c)` | :math:`|d| < |c|` |
|                                     |    \underbrace{(c_{i_1\dots i_n})}_c \rightarrow |                           |                   |
|                                     |    \underbrace{(d_{i_1\dots i_m})}_d             |                           |                   |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Unfolds
^^^^^^^

+-------------------------------------------------------------------------------------------+-----------------------------------------------+
| Unfold                                                                                    | Resulting sequence                            |
+----------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operators                              | Sequence transformation                          | Dimension                 | Length            |
+========================================+==================================================+===========================+===================+
| .. math::                              | .. math::                                        | :math:`\dim(c) > \dim(d)` | :math:`|c| > |d|` |
|    p: D &\rightarrow \mbox{Boolean} \\ |    \underbrace{(d_{i_1\dots i_m})}_d \rightarrow |                           |                   |
|    q: D &\rightarrow D \times C        |    \underbrace{(c_{i_1\dots i_n})}_c             |                           |                   |
+----------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Composite CHOFs
---------------
