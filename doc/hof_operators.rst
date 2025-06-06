HOF Operators
=============

In what follows, a sequence :math:`[a_i]_{i \in \iset{a}}` will often be represented by a single variable :math:`a`.
Whether a variable name (e.g. :math:`a`) represents a sequence or an operation to a higher-order function will be apparent based on context.

Transforms
----------

+------------------------------+----------------------------+------------------------+
| **Transform**                | Operator                   | Output sequence length |
+==============================+============================+========================+
| :math:`b = \transform{f}\ a` | :math:`f: A \rightarrow B` | :math:`|b| = |a|`      |
+------------------------------+----------------------------+------------------------+

The transform is the simplest HOF whose algorithms create data products.
Specifically, the algorithm :math:`f` is applied to each element of the input sequence :math:`a`, creating a corresponding data product in the output sequence :math:`b`:

.. math::

   \sequence{b}{a} = \sequence{f\ a} = \transform{f}\ \sequence{a}{a}

where :math:`b_i = f\ a_i`.
Note that the index set of the output sequence is the same as the index set of the input sequence.

.. todo::

   Allow for transforms where the output sequence is indexed by a different set—i.e. the number of elements remains the same as the input sequence, but the *label* of those elements changes.

.. table::
    :widths: 15 85

    +--------------+------------------------------------------------------------------+
    | **Operator** | **Allowed signature**                                            |
    +==============+==================================================================+
    | :math:`f`    | :cpp:`return_type function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+------------------------------------------------------------------+

**Return type**: A transform algorithm may create multiple data products by returning an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

To illustrate the different ways a transform's algorithm can be registered with Phlex, we use the following classes and functions, which are presumably defined in some experiment libraries.

.. code:: c++

   class geometry { ... };
   class hits { ... };
   class tracks { ... };
   class vertices { ... };

   tracks make_tracks(hits const&) { ... }
   vertices make_vertices(geometry const&, tracks const&) { ... }

**Transform with one argument (default output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     transform("track_maker", make_tracks, concurrency::unlimited)
       .sequence("good_hits"_in("spill"));
   }

**Transform with one argument (user-specified output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     products("good_tracks") =
       transform("track_maker", make_tracks, concurrency::unlimited)
       .sequence("good_hits"_in("spill"));
   }

**Transform with two arguments (default output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     transform("vertex_maker", make_vertices, concurrency::unlimited)
       .sequence("geometry"_in("job"), "good_hits"_in("spill"));
   }

Filters and Predicates
----------------------

+----------------------------+-----------------------------------------+------------------------+
| **Filter**                 | Operator (predicate)                    | Output sequence length |
+============================+=========================================+========================+
| :math:`a' = \filter{p}\ a` | :math:`p: A \rightarrow \text{Boolean}` | :math:`|a'| \le |a|`   |
+----------------------------+-----------------------------------------+------------------------+

Any user-defined algorithm or output sink may be configured to operate on data that satisfy a Boolean condition or *predicate*.
The act of restricting the invocation of a function to data that satisfy a predicate is known as *filtering*.
To filter data as presented to a given algorithm, one or more predicates must be specified in a *filter clause*.
Phlex will not schedule a predicate for execution if it is not bound to a filter.

.. todo::

   Define filter clause.
   Many algorithms can specify the same predicate in their filter clauses without executing the predicate multiple times.

Phlex will only schedule a filter for execution if there is at least one non-filter algorithm or output sink downstream of it.
Predicates can be evaluated on (e.g.) run-level data-product sets and applied to algorithms that process data from data-product sets that are subsets of the run (e.g. events).

.. table::
    :widths: 15 85

    +--------------+-----------------------------------------------------------+
    | **Operator** | **Allowed signature**                                     |
    +==============+===========================================================+
    | :math:`p`    | :cpp:`bool function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+-----------------------------------------------------------+


Registration interface
^^^^^^^^^^^^^^^^^^^^^^

.. code:: c++

   class tracks { ... };

   bool select_tracks(Tracks const& ts) { ... }

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto selected_data_scope = config.get<std::string>("data_scope");

     predicate("good_tracks", select_tracks, concurrency::unlimited)
       .sequence( <input_sequence_spec> );
   }

Observers
---------

+---------------------------------+--------------------------------------+------------------------+
| **Observer**                    | Operator                             | Output sequence length |
+=================================+======================================+========================+
| :math:`[\ \ ] = \observe{f}\ a` | :math:`f: A \rightarrow \mathbbm{1}` | :math:`0`              |
+---------------------------------+--------------------------------------+------------------------+

As mentioned in :numref:`functional_programming:Higher-order functions supported by Phlex`, observers are a special case of filters that always reject the data presented to them.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

.. table::
    :widths: 15 85

    +--------------+-----------------------------------------------------------+
    | **Operator** | **Allowed signature**                                     |
    +==============+===========================================================+
    | :math:`f`    | :cpp:`void function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+-----------------------------------------------------------+

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

The following classes and functions are presumed to be experiment-defined and are used to demonstrate the registration interface for observers:

.. code:: c++

   class geometry { ... };
   class tracks { ... };
   void check_tracks(tracks const&) { ... }
   void check_vertices(geometry const&, vertices const&) { ... }

**Observer with one argument**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     observe(check_tracks, concurrency::unlimited)
       .sequence("good_tracks"_in("spill"));
   }

**Observer with two arguments**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     observe(check_vertices, concurrency::unlimited)
       .sequence("geometry"_in("job"), "good_tracks"_in("spill"));
   }


Partitioned Folds
-----------------

+-------------------------------------------------------+----------------------------------------------------------------------+------------------------+
| **Partitioned fold**                                  | Operators                                                            | Output sequence length |
+=======================================================+======================================================================+========================+
| :math:`d = \fold{f}{\textit{init}}{\textit{part}}\ c` | :math:`f: D \times C \rightarrow D`                                  | :math:`|d| \le |c|`    |
|                                                       +----------------------------------------------------------------------+                        |
|                                                       | :math:`\textit{init}: \mathbbm{1} \rightarrow D`                     |                        |
|                                                       +----------------------------------------------------------------------+                        |
|                                                       | :math:`\textit{part}: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})` |                        |
+-------------------------------------------------------+----------------------------------------------------------------------+------------------------+

As mentioned in :numref:`functional_programming:Sequences of Data and Higher-Order Functions`, a *fold* can be defined as a transformation of a sequence of data to a single value:

.. math::
   d = \fold{f}{\textit{init}}\ \sequence{c}{c}

where the user-defined operation :math:`f` is applied repeatedly between an accumulated value (initialized by :math:`init`) and each element of the input sequence.

In a framework context, however, multiple fold results are often desired in the same program for the same kind of computation.
For example, consider a program that processes :math:`n` runs, each of which contains spills, identified by the tuple :math:`(R\ i, S\ j)`.
The user may wish to create one histogram per run that contains the track multiplicity per spill.
Instead of creating a single fold result, we thus use a *partitioned fold*:

.. math::
   :no-wrap:

   \begin{align*}
   [h_{(R\ 1)}&,\ \dots,\ h_{(R\ n)}] \\
              &= \fold{\textit{fill}}{\textit{init}}{\textit{into\_runs}}\ [m_{(R\ 1, S\ 1)},\ m_{(R\ 1, S\ 2)},\ \dots,\ m_{(R\ n, S\ 1)},\ m_{(R\ n, S\ 2)},\ \dots]
   \end{align*}

where :math:`h_{(R\ i)}` denotes the histogram for run :math:`i`, and :math:`m_{(R\ i,\ S\ j)}` is the track multiplicity for spill :math:`j` in run :math:`i`.

The above equation can be expressed more succinctly as:

.. math::
   [h_j]_{j \in \iset{\text{out}}} = \fold{\textit{fill}}{\textit{init}}{\textit{into\_runs}}\ [m_i]_{i \in \iset{\text{in}}}

where

.. math::
   :no-wrap:

   \begin{align*}
   \iset{\text{in}} &= \{(R\ 1,\ S\ 1),\ (R\ 1,\ S\ 2),\ \dots,\ (R\ n,\ S\ 1),\ (R\ n,\ S\ 2), \dots\}, \text{and}\\
   \iset{\text{out}} &= \{(R\ 1),\ \dots, (R\ n)\}\ .
   \end{align*}

Partitions
^^^^^^^^^^

Factorizing a set of data into non-overlapping subsets that collectively span the entire set is called creating a set *partition*. [Wiki-partition]_
Each subset of the partition is called a *cell*.
In the above example, the role of the :math:`\textit{into\_runs}` operation is to partition the input sequence into runs so that there is one fold result per run.
In general, however, the partitioning function is of the form :math:`\textit{part}: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})`, where:

- the domain is the singleton set that contains only the index set :math:`\iset{c}` (i.e. :math:`\textit{part}` can only be invoked on :math:`\iset{c}`), and
- the codomain is the set of partitions of the index set :math:`\iset{c}`.

The function :math:`part` also establishes an equivalence relationship on the index set :math:`\iset{c}`, where each element of the index set is mapped to a cell of the partition.
The number of elements in the output sequence :math:`d` corresponds to the number of partition cells.

Initializing the Accumulator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo::
   Change the domain type of :math:`\textit{init}`.

A crucial ingredient of the fold is the *accumulator*, which stores the fold result while it is being formed.
Each accumulator is initialized by invoking a user-defined operation :math:`\textit{init}: \mathbbm{1} \rightarrow D`, which returns an object that has the same type :math:`D` as the fold result. [#finit]_
Instead of invoking a function, an accumulator is often initialized with a value.
However, in functional programming, a value can be represented by invoking a function that always returns the same result.
Expressing an initializer as a function thus supports value-initialization while retaining the flexibility that may occasionally be required through functions.

Fold Operation
^^^^^^^^^^^^^^

A cell's fold result is obtained by repeatedly applying a fold operation to the cell's accumulator and each element of that cell's input sequence.
The fold operation has the signature :math:`f: D \times C \rightarrow D`, where :math:`D` represents the type of the accumulator/fold result, and :math:`C` is the type of each element of the input sequence.

In the above example, the function :math:`\textit{fill}` receives a histogram :math:`h_{(R\ i)}` as the accumulator for run :math:`i` and "combines" it with a track multiplicity object :math:`m_{(R\ i,\ S\ j)}` that belongs to spill :math:`j` in run :math:`i`.
This "combined" value is then returned by :math:`\textit{fill}` as the updated value of the accumulator.
The function :math:`\textit{fill}` is repeatedly invoked to update the accumulator with each track multiplicity value.
Once all track multiplcity values in run :math:`i` have been processed by :math:`\textit{fill}`, the accumulator's value becomes the fold result for that run.

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

**Result type**: A fold algorithm may create multiple data products through its result by specifying an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

.. table::
    :widths: 15 85

    +-----------------------+-------------------------------------------------------------------------+
    | **Operator**          | **Allowed signature**                                                   |
    +=======================+=========================================================================+
    | :math:`f`             | :cpp:`void function_name(result_type&, P1, Pn..., Rm...) [qualifiers];` |
    +-----------------------+-------------------------------------------------------------------------+
    | :math:`\textit{init}` | :cpp:`result_type{...}`                                                 |
    +-----------------------+-------------------------------------------------------------------------+
    | :math:`\textit{part}` | *Name of data-set category*                                             |
    +-----------------------+-------------------------------------------------------------------------+

Partitioned Unfolds
-------------------

+---------------------------------------------------------+----------------------------------------------------+------------------------+
| **Partitioned unfold**                                  | Operators                                          | Output sequence length |
+=========================================================+====================================================+========================+
| :math:`c = \unfold{p}{\textit{gen}}{\textit{label}}\ d` | :math:`p: D \rightarrow \mbox{Boolean}`            | :math:`|c| \ge |d|`    |
|                                                         +----------------------------------------------------+                        |
|                                                         | :math:`\textit{gen}: D \rightarrow D \times C`     |                        |
|                                                         +----------------------------------------------------+                        |
|                                                         | :math:`\textit{label}: \mathbbm{1} \rightarrow L`  |                        |
+---------------------------------------------------------+----------------------------------------------------+------------------------+

Unfolds are the opposite of folds, where the output sequence is larger than the input sequence :need:`DUNE 33`.
An unfold can be used for parallelizing the processing of a data product in smaller chunks.

Unfolding predicate
^^^^^^^^^^^^^^^^^^^

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

Composite CHOFs
---------------

.. rubric:: Footnotes

.. [#finit] It is acceptable for :math:`\textit{init}` to return a type that is convertible to the accumulator's type.

.. only:: html

   .. rubric:: References

.. [Wiki-partition] https://en.wikipedia.org/wiki/Partition_of_a_set
