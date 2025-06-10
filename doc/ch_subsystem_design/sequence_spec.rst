====================
Specifying Sequences
====================

As discussed in :numref:`ch_conceptual_design/registration:Framework Registration`, the input sequence to a HOF is specified via a :cpp:`sequence(...)` clause, where the ellipsis is replaced by a *sequence specification*.
One of the specifications provided is :cpp:`"good_hits"_in("spill")`, which indicates that a sequence should be formed from data products labeled :cpp:`"good_hits"` from each :cpp:`"spill"` data-set category.

Input sequence specification
----------------------------

A sequence specification contains one or more *product specifications* and zero or more *resource specifications*, expressed in EBNF grammar [Wiki-EBNF]_ as:

.. productionlist::
    input_sequence_spec : `input_product_specs` | `input_products` "," `resources`
    input_product_specs : Product | Product "," (`input_product_specs` | `resources`)
    input_products : Product | Product "," `input_products`
    resources : Resource | Resource "," `resources`

where :cpp:`Product` and :cpp:`Resource` specify a single product and a single resource, respectively.

In the code examples that follow, the bracketed term `<input_sequence_spec>` will refer to any specification allowed in the :cpp:`sequence(...)` clause.
The specifications for :cpp:`Product` and :cpp:`Resource` depend on the computing language of the algorithm and are defined elsewhere.

.. todo::

    Where will be define the :cpp:`Product` and :cpp:`Resource` specifications, and what are they?

Output sequence specification
-----------------------------

.. productionlist::
    output_sequence_spec : ProductOutputSpec | ProductOutputSpec "," `output_sequence_spec`

.. only:: html

   .. rubric:: References

.. [Wiki-EBNF] https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
