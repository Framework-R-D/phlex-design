**********************
Sequences and Families
**********************

A finite sequence can be represented as an ordered list of objects, all of which are from the same set or *type* (see :numref:`ch_preliminaries/types:Types`).
For example, the sequence :math:`a = [a_1, a_2, \dots, a_n]` is written as

.. math::
    a = [a_1, a_2, \dots, a_n] = [a_i]_{i \in \{1, 2, \dots, n\}} = \family{a}

where each :math:`a_i \in A`, and :math:`\mathcal{I} = \{1, 2, \dots, n\}` is the :term:`index set` whose elements are used to identify objects within the sequence :math:`a`.

The elements :math:`a_i` can be ordered only if the index set itself is ordered [#strict]_—e.g because the elements of the set :math:`\mathbb{N}` respect the order :math:`1 \lt 2 \lt 3 \lt \cdots` , the elements of :math:`a` are listed in that order, and :math:`a` is, therefore, a sequence.
However, if the index set has no ordering associated with it, then instead of :math:`a` representing a sequence, it represents an :term:`indexed family`, where the ordering of elements is not meaningful.
Specifically, an indexed family :math:`b` can be equivalently represented as:

.. math::
    b = [b_1, b_2, \dots, b_n] = [b_n, \dots, b_2, b_1] = \family{b}

or any of the :math:`n!` orderings possible.
The index set :math:`\isetdefault` simply provides a mapping from an index to the data object itself.

.. important::

   The order in which elements are presented to algorithms is not guaranteed in concurrent contexts.
   Phlex, therefore, processes indexed families of data and not sequences.

In some cases, it is convenient to represent a family's index as a tuple of numbers, thus representing layers of nesting in the data organization (see :numref:`ch_conceptual_design/data_organization:Data Organization`).
As an illustration, to use the RSE hierarchy from :numref:`introduction:Flexibility` one could specify

.. math::
    c = \ifamily{c}{\text{RSE}} = [c_{rse}]_{(r,s,e) \in \iset{\text{RSE}}}

to refer to a family of data products :product:`c`, where the variables :math:`r`, :math:`s`, and :math:`e` correspond to the identifiers for specific `Run`\ s, `Subrun`\ s, and `Event`\ s.
The values that :math:`r`, :math:`s`, and :math:`e` are permitted to take are determined by the members of the index set :math:`\iset{\text{RSE}}`, which contains triplets of numbers.
The semantics of the indices depend on the use case, and it is usually best to denote the semantics along with the indices (e.g.):

.. math::
    c = [c_{rse}]_{(\text{Run}\ r,\ \text{Subrun}\ s,\ \text{Event}\ e) \in \iset{\text{RSE}}}

This avoids confusion when other data organizations are considered.
For presentation purposes, however, we only include explicit semantics whenever it is necessary for describing the data-processing in question (e.g. :numref:`ch_conceptual_design/hofs/partitioned_folds:Partitioned Folds`).

The *length* of the family :math:`b` above is denoted by :math:`|b|`, and it equals the *cardinality* of the index set: :math:`|b| = |\isetdefault|`.
In this document, we also use the *empty family*, which has length 0, and whose elements can be indexed by the empty set:

.. math::
    [\ ] = [b_i]_{i \in \emptyset} \ .

We also use the notation :math:`\tilde{b}` to denote a family whose elements are either Boolean `true` (:math:`\textsf{T}`) or `false` (:math:`\textsf{F}`), such as :math:`\tilde{b} = [\textsf{T}, \textsf{T}, \textsf{F}, \dots]`.

.. rubric:: Footnotes

.. [#strict] Specifically, the index set must be equipped with a strict total order.
