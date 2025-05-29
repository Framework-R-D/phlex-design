*********
Sequences
*********

A finite sequence can be represented as a list of objects, each of which are of the same set or *type* (see :numref:`types:Types`).
For example, the sequence :math:`a = [a_1, a_2, \dots, a_n]` is written as

.. math::
    a = [a_1, a_2, \dots, a_n] = [a_i]_{i \in \{1, 2, \dots, n\}} = [a_i]_{i \in \mathcal{I}_a}

where each :math:`a_i \in A`, and :math:`\mathcal{I}_a = \{1, 2, \dots, n\}` is the :term:`index set` whose elements are used to identify objects within the sequence :math:`a`.

In general, an index set does not impose an order on the sequence elements---the sequence :math:`a` could thus be equivalently expressed as:

.. math::
    a = [a_1, a_2, \dots, a_n] = [a_n, \dots, a_2, a_1]

or any of the :math:`n!` orderings possible.
The index set simply provides a mapping from an index to the data object itself.

The *length* of the sequence :math:`a` above is denoted by :math:`|a|`, and it equals the *cardinality* of the index set: :math:`|a| = |\mathcal{I}_a| = n`.
In this document, we will have need of a list of length 0, or the empty list, which is a sequence whose elements are indexed by the empty set:

.. math::
    [\ ] = [a_i]_{i \in \emptyset} \ .
