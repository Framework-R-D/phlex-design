-------
Windows
-------

+---------------------------------------+---------------------------------------------------------+------------------------+
| **Window**                            | Operators                                               | Output sequence length |
+=======================================+=========================================================+========================+
| :math:`y = \window{f}{adj}{label}\ x` | :math:`f: X \times \opt{X} \rightarrow Y`              | :math:`|y| = |x|`      |
|                                       +---------------------------------------------------------+                        |
|                                       | :math:`adj: \iset{x} \times \iset{x} \rightarrow \bool` |                        |
|                                       +---------------------------------------------------------+                        |
|                                       | :math:`label: \one \rightarrow L`                       |                        |
+---------------------------------------+---------------------------------------------------------+------------------------+

One of the unique capabilities of Phlex is to execute an algorithm on data products that belong to adjacent data-product sets (see :numref:`ch_conceptual_design/registration:Data Products from Adjacent Data-Product Sets`).
The workflow in :numref:`workflow` shows a such a node :math:`\textit{window(make\_tracks)}`, which is presented with pairs of :cpp:`"GoodHits"` data products, with each data product in the pair belonging to adjacent `APA`\ s.
It is the user-provided :math:`adj` function which determines whether two data-product sets are adjacent.

For simplicity, imagine that each `APA` identifier (i.e. member of the set :math:`\iset{\text{APA}}`) can be represented as an integer.
A straightforward :math:`adj` implementation might be to group the :cpp:`"GoodHits"` data products from `APA`\ s with consecutive numbers:

.. only:: html

     .. math::

         \isequence{hs}{\text{APA}} = [\ \rlap{\overbracket{\phantom{hs_1 \ \ hs_2}}^{a}\ \ \overbracket{\phantom{hs_3\ \ hs_4}}^{c}}hs_1, \underbracket{hs_2,\  hs_3}_{b},\ hs_4,\ \dots,\ \underbracket{hs_{n-1},\ hs_n}_{m}\ ]

.. only:: latex

     .. math::

         \isequence{hs}{\text{APA}} = [\ \rlap{$\overbracket{\phantom{hs_1 \ \ hs_2}}^{a}\ \ \overbracket{\phantom{hs_3\ \ hs_4}}^{c}$}hs_1, \underbracket{hs_2,\  hs_3}_{b},\ hs_4,\ \dots,\ \underbracket{hs_{n-1},\ hs_n}_{m}\ ]

The data products corresponding to windows :math:`a` through :math:`m` are grouped into pairs and presented to an algorithm :math:`make\_tracks'`, which has the signature :math:`\text{Hits} \times \text{Hits} \rightarrow \text{Tracks}`.
There are, at most, :math:`n-1` unique pairs that can be presented to the function :math:`make\_tracks'` such that:

.. math::

    \left[ts_i\right]_{i \in \iset{\text{APA}}'} = \left[make\_tracks'(hs_i, hs_{i+1})\right]_{i \in \iset{\text{APA}}'} = \window{make\_tracks'}{adj}{\text{APA}} \isequence{hs}{\text{APA}}

where the index set :math:`\iset{\text{APA}}'` is :math:`\iset{\text{APA}}` without the last identifier :math:`n` [#flast]_.
In this example, the identifier of the first :math:`hs` object in the pair is used to identify the tracks collection :math:`ts`.
But Phlex does not mandate this choice, and a different data category could be specified by the :math:`label` operator for the data products of the output sequence.

Operator Signatures
^^^^^^^^^^^^^^^^^^^

One limitation of the above formulation is that index sets of the input and output sequences are not the same.
To address this infelicity, the function signature of :math:`make\_tracks'` can be adjusted such that the second argument receives an optional type.
We call this new algorithm :math:`make\_tracks`:

.. math::

   make\_tracks: \text{Hits} \times \opt{\text{Hits}} \rightarrow \text{Tracks}

thus permitting symmetry between the input and output data-product sequences:

.. math::

   \isequence{ts}{\text{APA}} &= \window{make\_tracks}{adj}{label} \isequence{hs}{\text{APA}} \\
   &=\left[make\_tracks(hs_i, hs_{i+1})\right]_{i \in \iset{\text{APA}}'} \boldsymbol{+}\ \left[make\_tracks(hs_n, ())\right]

where :math:`label` returns the value of `APA`, :math:`\boldsymbol{+}` is the list-concatenation operator, and :math:`()` is the null value.
Phlex supports the function signature whose second argument is an optional type :math:`\opt{X}`.

.. table::
    :widths: 12 88

    +---------------+------------------------------------------------------------------------------------+
    | **Operator**  | **Allowed signature**                                                              |
    +===============+====================================================================================+
    | :math:`f`     | :cpp:`return_type function_name(P1, Opt<P2>, Rm...) [quals];`                      |
    +---------------+------------------------------------------------------------------------------------+
    | :math:`adj`   | :cpp:`bool function_name(<P1 identifier>, <P2 identifier>) [quals];`               |
    +---------------+------------------------------------------------------------------------------------+
    | :math:`label` | *Name of data-set category of output data products*                                |
    +---------------+------------------------------------------------------------------------------------+

The :cpp:`return_type` must model the created data-product type described in :numref:`ch_conceptual_design/algorithms:Return Types`.
The algorithm :math:`f` may also create multiple data products by returning a :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a created data-product type.

The second argument :cpp:`Opt<P2>` indicates that an optional type is passed to the algorithm.
It is permitted to use resources (i.e. :cpp:`Rm...`) in the function :math:`f`.
The data-product set identifers of :cpp:`P1` and :cpp:`P2` are used to determine whether two data-products reside in adjacent data-product sets.

Registration Interface
^^^^^^^^^^^^^^^^^^^^^^

The :math:`\textit{window(make\_tracks)}` node in :numref:`workflow` would be represented in C++ as:

.. code:: c++

   class hits { ... };
   class tracks { ... };
   class id { ... };
   tracks make_tracks(tracks const& ts, tracks const* next_ts) { ... }
   bool are_adjacent(id const& left, id const& right) { ... }

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("GoodTracks") =
       window(
         "track_maker",          // <= Node name for framework
         make_tracks,            // <= Window algorithm (f)
         are_adjacent            // <= Adjacency criterion
         "APA",                  // <= Output data-product category
         concurrency::unlimited  // <= Allowed concurrency
       )
       .sequence("GoodHits"_in("APA"));
   }

Note that the second input parameter for :cpp:`make_tracks` is an optional type.
The type :cpp:`id` is a metadata type (possibly defined by the experiment) that enables the comparison of data-product identifiers for establishing adjacency.

.. rubric:: Footnotes

.. [#flast] The expression :math:`f(hs_n, hs_{n+1})` is ill-formed as there are only :math:`n` elements in the set :math:`\iset{\text{APA}}`.
