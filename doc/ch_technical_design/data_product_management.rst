Data Product Management
=======================

.. todo::
   - Data cell definitions
   - Data family definitions
   - Data product definitions
   - Data product specifications
   - Data-product lookup policies
   - Describes what data products should be used/created by an algorithm
   - Describes how the :math:`\textit{label}` operator can be used (as described in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions`)


Specifying Familiies
--------------------

As discussed in :numref:`ch_conceptual_design/registration:Framework Registration`, the input family to a HOF is specified via a :cpp:`family(...)` clause, where the ellipsis is replaced by a *family specification*.
One of the specifications provided is :cpp:`product_selector{.suffix = "good_hits", .layer = "spill"}`, which indicates that a family should be formed from data products labeled :cpp:`"good_hits"` from each :cpp:`"spill"` data layer.

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



Semantics and lookup policy for `product_selector`
--------------------------------------------------

1. Only the data-product concept will be unconditionally required, and that could be calculated by the framework.
2. It is a framework-graph initialization error if the `product_selector` object resolves to multiple data products.
3. Ambiguities can be resolved by specifying additional fields.
4. This implies that a working Phlex job may fail once a user adds another data product whose specification matches an existing `product_selector`.
5. Duplicate specifications, as resolved from `product_selector` objects, of the same input data product will be allowed.
6. A `product_selector` with an absent layer specification implies that any data products matching the query will be presented to the node.
7. The data layers of data products processed by an algorithm do not necessarily need to be known before forming the data-flow graph.
8. If the fields in the `product_selector` are sufficient to unambiguously connect one node to another, the data layer does not need to be known ahead of time.
9. The user must specify a product's data layer, or equivalent disambiguating information using other `product_selector` fields, to guarantee that only products associated with that layer will be processed.

Data-layer semantics for transforms
-----------------------------------

If a transform algorithm receives input data products from unrelated data layers (data layers in different hierarchies) , the data product created by the algorithm will be associated with every input data layer.

Identifier restrictions in product specification
------------------------------------------------

Phlex will enforce the restriction that the following identifiers used in specifying a data product must be representable as a valid C++ variable name:

   - plugin name
   - node name
   - suffix
   - stage name
   - data-layer name element

.. only:: html

   .. rubric:: References

.. [Wiki-EBNF] https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
