-----------------
Partitioned Folds
-----------------

+--------------------------------------------------------+----------------------------------------------------------------------+------------------------+
| **Partitioned fold**                                   | Operators                                                            | Output sequence length |
+========================================================+======================================================================+========================+
| :math:`d = \pfold{f}{\textit{init}}{\textit{part}}\ c` | :math:`f: D \times C \rightarrow D`                                  | :math:`|d| \le |c|`    |
|                                                        +----------------------------------------------------------------------+                        |
|                                                        | :math:`\textit{init}: \one \rightarrow D`                            |                        |
|                                                        +----------------------------------------------------------------------+                        |
|                                                        | :math:`\textit{part}: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})` |                        |
+--------------------------------------------------------+----------------------------------------------------------------------+------------------------+

As mentioned in :numref:`ch_preliminaries/functional_programming:Sequences of Data and Higher-Order Functions`, a *fold* can be defined as a transformation of a sequence of data to a single value:

.. math::
   d = \fold{f}{\textit{init}}\ \isequence{c}{c}

where the user-defined operation :math:`f` is applied repeatedly between an accumulated value (initialized by :math:`init`) and each element of the input sequence.

In a framework context, however, multiple fold results are often desired in the same program for the same kind of computation.
For example, consider a program that processes :math:`n` runs, each of which contains spills, identified by the tuple :math:`(R\ i, S\ j)`.
The user may wish to create one histogram per run that contains the track multiplicity per spill.
Instead of creating a single fold result, we thus use a *partitioned fold*:

.. math::
   :no-wrap:

   \begin{align*}
   [h_{(R\ 1)}&,\ \dots,\ h_{(R\ n)}] \\
              &= \pfold{\textit{fill}}{\textit{init}}{\textit{into\_runs}}\ [m_{(R\ 1, S\ 1)},\ m_{(R\ 1, S\ 2)},\ \dots,\ m_{(R\ n, S\ 1)},\ m_{(R\ n, S\ 2)},\ \dots]
   \end{align*}

where :math:`h_{(R\ i)}` denotes the histogram for run :math:`i`, and :math:`m_{(R\ i,\ S\ j)}` is the track multiplicity for spill :math:`j` in run :math:`i`.

The above equation can be expressed more succinctly as:

.. math::
   [h_j]_{j \in \iset{\text{out}}} = \pfold{\textit{fill}}{\textit{init}}{\textit{into\_runs}}\ [m_i]_{i \in \iset{\text{in}}}

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
Each accumulator is initialized by invoking a user-defined operation :math:`\textit{init}: \one \rightarrow D`, which returns an object that has the same type :math:`D` as the fold result. [#finit]_
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

.. rubric:: Footnotes

.. [#finit] It is acceptable for :math:`\textit{init}` to return a type that is convertible to the accumulator's type.

.. only:: html

   .. rubric:: References

.. [Wiki-partition] https://en.wikipedia.org/wiki/Partition_of_a_set
