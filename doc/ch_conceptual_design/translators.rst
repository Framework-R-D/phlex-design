Translators
===========

A translator function converts an object of one :term:`concrete data-product type <Concrete data-product type>` into an object of a different concrete type that models the same :term:`data-product concept <Data-product concept>`.
A translator node runs such a function in the computational graph. The function's registration and the node that executes it are distinct.

A single data-product concept may have multiple concrete representations (e.g. different language bindings, GPU- and CPU-friendly layouts).
Conversely, a single concrete type may model multiple data-product concepts. For example, a :cpp:`double` alone does not tell the framework whether a value represents protons on target or an energy measurement.

The agreed design is for the framework to insert translator nodes where needed, based on the concepts and concrete types of the requested and available products.
This is not the behavior of the explicit translator path under review; see :numref:`ch_technical_design/task_management:Translators`.
