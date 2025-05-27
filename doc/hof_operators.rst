HOF Operators
=============

Transforms
----------

+------------------------------+----------------------------+------------------------+
| **Transform**                | Operator                   | Output sequence length |
+==============================+============================+========================+
| :math:`b = \transform{f}\ a` | :math:`f: A \rightarrow B` | :math:`|b| = |a|`      |
+------------------------------+----------------------------+------------------------+

**Return type**: A transform algorithm may create multiple data products by returning an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

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

Unfolds are the opposite of folds, where the output sequence is larger than the input sequence :dune:`17 Unfolding data products`.
An unfold can be used for parallelizing the processing of a data product in smaller chunks.

.. todo:: Explain predicate unfolds here.

Composite CHOFs
---------------

.. rubric:: Footnotes

.. [#finit] It is acceptable for :math:`\textit{init}` to return a type that is convertible to the accumulator's type.

.. only:: html

   .. rubric:: References

.. [Wiki-partition] https://en.wikipedia.org/wiki/Partition_of_a_set
