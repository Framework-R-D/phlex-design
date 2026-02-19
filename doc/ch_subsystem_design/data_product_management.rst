Data Product Management
=======================

.. todo::
   - Data (product) set definitions
   - Data family definitions
   - Data product definitions
   - Data product specifications
   - Data-product lookup policies
   - Describes what data products should be used/created by an algorithm
   - Describes how the :math:`\textit{label}` operator can be used (as described in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions`)


Specifying Familiies
--------------------

As discussed in :numref:`ch_conceptual_design/registration:Framework Registration`, the input family to a HOF is specified via a :cpp:`family(...)` clause, where the ellipsis is replaced by a *family specification*.
One of the specifications provided is :cpp:`"good_hits"_in("spill")`, which indicates that a family should be formed from data products labeled :cpp:`"good_hits"` from each :cpp:`"spill"` data layer.

Input Family Specification
--------------------------

A family specification contains one or more *product specifications* and zero or more *resource specifications*, expressed in EBNF grammar [Wiki-EBNF]_ as:

.. productionlist::
    input_family_spec : `input_product_specs` | `input_products` "," `resources`
    input_product_specs : Product | Product "," (`input_product_specs` | `resources`)
    input_products : Product | Product "," `input_products`
    resources : Resource | Resource "," `resources`

where :cpp:`Product` and :cpp:`Resource` specify a single product and a single resource, respectively.

In the code examples that follow, the bracketed term `<input_family_spec>` will refer to any specification allowed in the :cpp:`family(...)` clause.
The specifications for :cpp:`Product` and :cpp:`Resource` depend on the computing language of the algorithm and are defined elsewhere.

.. todo::

    Where will be define the :cpp:`Product` and :cpp:`Resource` specifications, and what are they?

Output family specification
---------------------------

.. productionlist::
    output_family_spec : ProductOutputSpec | ProductOutputSpec "," `output_family_spec`

.. only:: html

   .. rubric:: References

.. [Wiki-EBNF] https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
