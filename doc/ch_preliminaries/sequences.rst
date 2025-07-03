*********
Sequences
*********

A finite sequence can be represented as a list of objects, each of which are of the same set or *type* (see :numref:`ch_preliminaries/types:Types`).
For example, the sequence :math:`a = [a_1, a_2, \dots, a_n]` is written as

.. math::
    a = [a_1, a_2, \dots, a_n] = [a_i]_{i \in \{1, 2, \dots, n\}} = [a_i]_{i \in \mathcal{I}_a}

where each :math:`a_i \in A`, and :math:`\mathcal{I}_a = \{1, 2, \dots, n\}` is the :term:`index set` whose elements are used to identify objects within the sequence :math:`a`.

In general, an index set can impose an order on the sequence elements only if the index set itself is ordered.
Because the elements of the set :math:`\mathbb{N}` respect the order :math:`1 \lt 2 \lt 3 \lt \cdots` , the sequence elements are listed in that order.
However, if the index set is assumed to have no ordering associated with it, then instead of :math:`a` representing a sequence, it represents an *indexed family*, where the ordering of elements is irrelevant.
Specifically, an indexed family :math:`a` can be equivalently represented as:

.. math::
    a = [a_1, a_2, \dots, a_n] = [a_n, \dots, a_2, a_1]

or any of the :math:`n!` orderings possible.
The index set simply provides a mapping from an index to the data object itself.

.. note::

   The order in which elements are presented to algorithms is not guaranteed in concurrent contexts.
   Phlex, therefore, formally processes indexed families of data, and not sequences.
   To avoid confusion with data families (:numref:`ch_conceptual_design/data_organization:Data Categories, Families, and Data-Product Sets`), we hereafter use the term *sequence* to refer to indexed families.
   When necessary, we will use the term *ordered sequence* to denote a formal mathematical sequence.

The *length* of the sequence :math:`a` above is denoted by :math:`|a|`, and it equals the *cardinality* of the index set: :math:`|a| = |\mathcal{I}_a| = n`.
In this document, we will have need of a list of length 0, or the empty list, which is a sequence whose elements are indexed by the empty set:

.. math::
    [\ ] = [a_i]_{i \in \emptyset} \ .

We also use the notation :math:`\tilde{a}` to denote a sequence whose elements are either Boolean `true` (:math:`\textsf{T}`) or `false` (:math:`\textsf{F}`), such as :math:`\tilde{a} = [\textsf{T}, \textsf{T}, \textsf{F}, \dots]`.
