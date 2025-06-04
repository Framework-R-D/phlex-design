====================
Specifying Sequences
====================

As discussed in :numref:`registration:Framework Registration`, the input sequence to a HOF is specified via a :cpp:`sequence(...)` clause, where the ellipsis is replaced by a *sequence specification*.
One of the specifications provided is :cpp:`"good_hits"_in("spill")`, which indicates that a sequence should be formed from data products labeled :cpp:`"good_hits"` from each :cpp:`"spill"` data-set category.

A sequence specification contains one or more *product specifications* and zero or more *resource specifications*, expressed in EBNF grammar [Wiki-EBNF]_ as:

.. productionlist::
    <sequence_spec> : `product_specs` | `products` "," `resource_specs`
    product_specs : Product | Product "," (`product_specs` | `resource_specs`)
    resource_specs : Resource | Resource "," `resource_specs`
    products : Product | Product "," `products`

where :cpp:`Product` and :cpp:`Resource` specify a single product and a single resource, respectively.

In the code examples that follow, the term `<sequence_spec>` refers to any specification allowed in the :cpp:`sequence(...)` clause.
The specifications for :cpp:`Product` and :cpp:`Resource` depend on the computing language of the algorithm and are defined elsewhere.

.. todo::

    Where will be define the :cpp:`Product` and :cpp:`Resource` specifications, and what are they?

.. only:: html

   .. rubric:: References

.. [Wiki-EBNF] https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
