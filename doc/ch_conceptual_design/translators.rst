Translators
===========

Translator convert an object of one :term:`concrete data product <Concrete data product>` and outputs an object of a different concrete data type that models the same :term:`data-product concept <Data-product concept>`.

A single data-product concept may have multiple concrete representations (e.g. different language bindings, GPU- and CPU-friendly layouts).
Conversely, a single concrete type may represent multiple data-product concepts (e.g., the concept of Protons On Target (POT) can be represened by a :cpp:`double` or a :cpp:`long`).

Translators are automatically added to the graph as necessary.
