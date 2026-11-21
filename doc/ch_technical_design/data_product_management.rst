Data Product Management
=======================

.. todo::
   - Data cell definitions
   - Data family definitions
   - Data product definitions
   - Describes what data products should be used/created by an algorithm
   - Describes how the :math:`\textit{label}` operator can be used (as described in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions`)


Specifying Families
-------------------

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

The C++ :cpp:`product_specification` holds the creator, suffix, and type used as an exact key within a product store.
The complete identifying context also includes the layer and data-cell context and the processing stage.
The creator is required; the suffix is either explicitly empty or named. An unspecified type is an exact value, not a wildcard.
The textual form is ``creator/suffix``, including the slash in ``creator/`` for an empty suffix. The type is not serialized by that text form.

A :cpp:`product_selector` is a set of matching criteria, not an exact product key.
An omitted suffix criterion accepts any suffix, :cpp:`.suffix = ""` matches only an empty suffix, and a named suffix matches only the same name.
An omitted creator criterion accepts any creator. In the current implementation, a supplied creator criterion can match either the plugin label or the algorithm component; the long-term rule is open in `Phlex #1094 <https://github.com/Framework-R-D/phlex/issues/1094>`_.

An omitted layer criterion imposes no layer filter where omission is supported. That does not guarantee that a provider with an unresolved input layer will receive indices from all layers: the current unresolved route receives no concrete indices. See `Phlex #1082 <https://github.com/Framework-R-D/phlex/issues/1082>`_.

The current producer catalog rejects competing matches that differ in creator, suffix, or type. It can accept several candidates with the same creator, suffix, and type, whose layers may differ.
Adding a criterion can resolve ambiguity. The broader concept-aware policy in the `Phlex #391 agreement <https://github.com/Framework-R-D/phlex/issues/391#issuecomment-4034321916>`_ is a design target, not a claim about the current catalog.
Separate inputs may intentionally select the same product; this does not mean one input can silently choose among ambiguous matches.

Data-layer semantics for transforms
-----------------------------------

If a transform algorithm receives input data products from unrelated data layers (data layers in different hierarchy paths) , the data product created by the algorithm will be associated with every input data layer.

Identifier restrictions in product specifications
--------------------------------------------------

A Phlex :term:`identifier` starts with an ASCII letter and contains only ASCII letters, digits, or underscores after that. C++ and Python keywords are allowed.
This rule applies to each named component of an algorithm name, a non-empty suffix, a stage name, and a data-layer name element. An absent plugin label and an explicit empty suffix are not identifiers.
A qualified algorithm name is ``plugin_label:algorithm``; an unqualified name is just ``algorithm``. See ADR 0001 for the distinction between construction, validation, and matching.

``CURRENT`` is lexically valid and can stand for the current stage in selectors and explicit provider output declarations.
It is reserved rather than a valid name for the running stage or a stage advertised by an implicit provider.

Borrowed conversion results
---------------------------

A borrowed conversion result refers to storage owned by the input product. Keeping the result object or its data-cell index alive does not keep that storage alive.
The conversion record's declaration alone does not retain the source. The explicit path under review does not extend source lifetime on behalf of a borrowed result.
Proposed ADR 0003 would retain the input store only for borrowed results, including across chains of borrowing conversions. This would be a lifetime guarantee, not a provenance field.

.. only:: html

   .. rubric:: References

.. [Wiki-EBNF] https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
