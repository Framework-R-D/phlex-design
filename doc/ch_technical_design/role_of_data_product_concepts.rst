The Role of Data Product Concepts
=================================

A :term:`data-product concept <Data-product concept>` names an abstraction, not a particular in-memory representation.
A concept can be modeled by several concrete types, and a concrete type can model several concepts.
The concrete type alone therefore does not uniquely identify a concept.

Concept registration
--------------------

The experimental model provides an owning registry keyed by the immutable, non-empty name of each concept.
An existing concrete type can be associated with a concept without changing that type's definition.
Registering a second concept with the same name adds its concrete types to the existing concept rather than replacing it; adding an existing member is idempotent.
Membership queries name the concept explicitly. There is no reverse lookup that chooses a preferred concept for a type.

Registry operations synchronize access made through the registry, not access through a concept pointer after lookup returns.
Callers must coordinate such access with registry writes and other mutation of the concept.
This model API does not establish who owns the registry at the graph level or how graph input ports acquire concept information.

Conversion records
------------------

Conversion records belong to the data-product concept whose concrete types they connect.
Each record contains the conversion name, the payload for the conversion function, and a declaration of whether the result owns or borrows its storage.
The record is keyed by the ordered pair of source and target concrete types. These types must be distinct and must both already model the concept.
A concept allows at most one record for each ordered pair; a conversion in the reverse direction has its own record.
Duplicate concept registration merges concrete types, not conversion records.

Storing a conversion record checks the type pair, but does not check whether the payload is callable or has the expected signature, enforce the declared result lifetime, or create a translator node.
See :ref:`translator-node-execution` for how translator nodes execute conversions.

.. todo::
   - Describe how IO uses and persists data-product concepts.
   - Describe how the IDL and code generation support data-product concepts.
