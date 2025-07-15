-----------------
Partitioned Folds
-----------------

+--------------------------------------------------------+----------------------------------------------------------------------+----------------------+
| **Partitioned fold**                                   | Operators                                                            | Output family length |
+========================================================+======================================================================+======================+
| :math:`d = \pfold{f}{\textit{init}}{\textit{part}}\ c` | :math:`f: D \times C \rightarrow D`                                  | :math:`|d| \le |c|`  |
|                                                        +----------------------------------------------------------------------+                      |
|                                                        | :math:`\textit{init}: \opt{\iset{d}} \rightarrow D`                  |                      |
|                                                        +----------------------------------------------------------------------+                      |
|                                                        | :math:`\textit{part}: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})` |                      |
+--------------------------------------------------------+----------------------------------------------------------------------+----------------------+

As mentioned in :numref:`ch_preliminaries/functional_programming:Families of Data and Higher-Order Functions`, a *fold* can be defined as a transformation of a family of data to a single value:

.. math::
   d = \fold{f}{\textit{init}}\ \ifamily{c}{c}

where the user-defined operation :math:`f` is applied repeatedly between an accumulated value (initialized by :math:`init`) and each element of the input family.

In a framework context, however, multiple fold results are often desired in the same program for the same kind of computation.
Consider the workflow in :numref:`workflow`, which processes `Spill`\ s, identified by the index :math:`j` or, more specifically, the tuple :math:`(S\ j)`.
Each `Spill` is unfolded into a family of `APA`\ s, which are identified by the pair of indices :math:`jk` or, more specifically, the tuple :math:`(S\ j, A\ k)`.
The energies of the :product:`GoodHits` data products in :numref:`workflow` are summed across `APA`\ s per `Spill` using the :mathfunc:`fold(sum_energy)` node.

Instead of creating one fold result, we thus use a *partitioned fold* to create one summed energy data-product per `Spill`:

.. math::
   :no-wrap:

   \begin{align*}
   [E_{(S\ 1)}&,\ \dots,\ E_{(S\ n)}] \\
              &= \pfold{\textit{sum\_energy}}{\textit{init}}{\textit{into\_spills}}\ [hs_{(S\ 1,\ A\ 1)},\ hs_{(S\ 1,\ A\ 2)},\ \dots,\ hs_{(S\ n,\ A\ 1)},\ hs_{(S\ n,\ A\ 2)},\ \dots]
   \end{align*}

where :math:`E_{(S\ j)}` denotes the :product:`TotalHitEnergy` data product for `Spill` :math:`j`, and :math:`hs_{(S\ j,\ A\ k)}` is the :product:`GoodHits` data product for `APA` :math:`k` in `Spill` :math:`j`.

The above equation can be expressed more succinctly as:

.. math::
   [E_j]_{j \in \iset{\text{out}}} = \pfold{\textit{sum\_energy}}{\textit{init}}{\textit{into\_runs}}\ [hs_i]_{i \in \iset{\text{in}}}

where

.. math::
   :no-wrap:

   \begin{align*}
   \iset{\text{in}} &= \{(S\ 1,\ A\ 1),\ (S\ 1,\ A\ 2),\ \dots,\ (S\ n,\ A\ 1),\ (S\ n,\ A\ 2), \dots\}, \text{and}\\
   \iset{\text{out}} &= \{(S\ 1),\ \dots, (S\ n)\}\ .
   \end{align*}

Partitions
^^^^^^^^^^

Factorizing a set of data into non-overlapping subsets that collectively span the entire set is called creating a set *partition* [Wiki-Partition]_.
Each subset of the partition is called a *cell*.
In the above example, the role of the :mathfunc:`into_spills` operation is to partition the input family into `Spill`\ s so that there is one fold result per `Spill`.
In general, however, the partitioning function is of the form :math:`\textit{part}: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})`, where:

- the domain is the singleton set that contains only the index set :math:`\iset{c}` (i.e. :mathfunc:`part` can only be invoked on :math:`\iset{c}`), and
- the codomain is the set of partitions of :math:`\iset{c}` or :math:`\mathbb{P}(\iset{c})`; note that the output index set :math:`\iset{d} \in \mathbb{P}(\iset{c})`.

The function :mathfunc:`part` also establishes an equivalence relationship on the index set :math:`\iset{c}`, where each element of the index set is mapped to a cell of the partition.
The number of elements in the output family :math:`d` corresponds to the number of partition cells.

As of this writing, the only partitions supported are those that correspond to the names of data cell categories.
The partition :mathfunc:`into_spills` can thus be represented by the string :cpp:`"Spill"`, which denotes that there is one partition spell per `Spill`.

Initializing the Accumulator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A crucial ingredient of the fold is the *accumulator*, which stores the fold result while it is being formed.
Each accumulator is initialized by invoking a user-defined operation :math:`\textit{init}: \opt{\iset{d}} \rightarrow D`, which returns an object that has the same type :math:`D` as the fold result [#finit]_.
The :math:`\opt{\iset{d}}` domain means that:

1. :mathfunc:`init` can receive an argument corresponding to the identifier of a cell, which is a member of the output index set :math:`\iset{d}`.
   In the example above, the relevant identifier would be that of the `Spill`–i.e. :math:`(S\ j)`.
2. :mathfunc:`init` can be invoked with no arguments, thus producing the same value each time the accumulator is initialized.
   This is equivalent to initializing the accumulator with a constant value.

The implementation of :mathfunc:`init` for the total good-hits energy fold results is to return the constant :math:`0`.

Fold Operation
^^^^^^^^^^^^^^

A cell's fold result is obtained by repeatedly applying a fold operation to the cell's accumulator and each element of that cell's input family.
The fold operation has the signature :math:`f: D \times C \rightarrow D`, where :math:`D` represents the type of the accumulator/fold result, and :math:`C` is the type of each element of the input family.

In the above example, the function :mathfunc:`sum_energy` receives a floating-point number :math:`E_{(S\ i)}`, representing the accumulated good-hits energy for `Spill` :math:`j` and "combines" it with the good-hits object :math:`hs_{(S\ j,\ A\ k)}` that belongs to `APA` :math:`k` in spill :math:`j`.
This combination involves calculating the energy represented by the :product:`GoodHits` data product :math:`hs_{(S\ j,\ A\ k)}` and adding that to the accumulated value.
This "combined" value is then returned by :mathfunc:`sum_energy` as the updated value of the accumulator [#feff]_.
The function :mathfunc:`sum_energy` is repeatedly invoked to update the accumulator with the :product:`GoodHits` data product.
Once all :product:`GoodHits` data products in `Spill` :math:`j` have been processed by :mathfunc:`sum_energy`, the accumulator's value becomes the fold result for that `Spill`.

Operator Signatures
^^^^^^^^^^^^^^^^^^^

.. table::
    :widths: 15 13 72

    +-----------------------+---------------------------------------------------------------------------------+
    | **Operator**          | **Allowed signature**                                                           |
    +=======================+=================================================================================+
    | :math:`f`             | :cpp:`void function_name(result_type&, P1, Pn..., Rm...) [quals];`              |
    +-----------------------+----------------+----------------------------------------------------------------+
    | :math:`\textit{init}` | *as constant:* | :cpp:`result_type{...}`                                        |
    |                       +----------------+----------------------------------------------------------------+
    |                       | *as function:* | :cpp:`result_type function_name() [quals];`                    |
    |                       +----------------+----------------------------------------------------------------+
    |                       | *as function:* | :cpp:`result_type function_name( <cell identifier> ) [quals];` |
    +-----------------------+----------------+----------------------------------------------------------------+
    | :math:`\textit{part}` | *Name of data layer for output data product*                                    |
    +-----------------------+---------------------------------------------------------------------------------+

The fold's :cpp:`result_type` must model the created data-product type described in :numref:`ch_conceptual_design/algorithms:Return Types`.
A fold algorithm may also create multiple data products by using a :cpp:`result_type` of :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a created data-product type.


Registration Interface
^^^^^^^^^^^^^^^^^^^^^^

The :mathfunc:`fold(sum_energies)` node in :numref:`workflow` would be represented in C++ as:

.. code:: c++

   void sum_energy(double& total_hit_energy, hits const& hs) { ... }

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("TotalHitEnergy") =
       fold(
         "sum_hit_energy",       // <= Node name for framework
         sum_energy,             // <= Fold operation
         0.,                     // <= Initializer for each fold result
         "Spill",                // <= Partition level (one fold result per Spill)
         concurrency::unlimited  // <= Allowed concurrency
       )
       .family("GoodHits"_in("APA"));
   }

In order for the user-defined algorithm :cpp:`sum_energy` algorithm to be safely executed concurrently, protections must be in place to avoid data races when updating the :cpp:`total_hit_energy` result object from multiple threads.
Possible solutions include using :cpp:`std::atomic_ref<double>` [#fatomicref]_, placing a lock around the operation that updates :cpp:`total_hit_energy` (less desirable due to inefficiencies), or perhaps using :cpp:`std::atomic<double>` [#fatomic]_ instead of  :cpp:`double` to represent the data product.

.. rubric:: Footnotes

.. [#finit] It is acceptable for :mathfunc:`init` to return a type that is convertible to the accumulator's type.
.. [#feff] Returning an updated accumulated value is generally not the most memory-efficient approach as it requires at least two copies of an accumulated value to be in memory at one time.
           The approach adopted by Phlex is to include a reference to the accumulated value as part of the fold operator's signature.
           The accumulator can then be updated in place, thus avoiding the extra copies of the data.
.. [#fatomicref] https://en.cppreference.com/w/cpp/atomic/atomic_ref.html
.. [#fatomic] https://en.cppreference.com/w/cpp/atomic/atomic.html


.. only:: html

   .. rubric:: References

.. [Wiki-Partition] https://en.wikipedia.org/wiki/Partition_of_a_set
