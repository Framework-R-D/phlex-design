## The Nature of Translator Nodes

A *translator  node* is a node whose input is of one concrete data-product type and whose output is of a different concrete data-product type, with both types modeling the same data-product concept.

1. Each translator node uses a single [translator function](the-nature-of-translator-functions).
2. A translator node is automatically inserted into the graph between two nodes whose data-product types are concept-compatible but not identical.
3. Translator nodes belong to the computational graph.
4. There are no translator nodes that precede a provider.
5. There are no translator nodes that follow a preserver.
6. There should never be two translator nodes that take the same input concrete data product and produce the same output concrete data product.
7. Translator nodes are not configured directly by the framework user; they must be discovered by the framework based on needs implied by the user configuration.
8. The translator metadata required for graph building should be generated at the time the translator plugin is compiled and linked.
9. Translator nodes record in the metadata associated with their output product that they created the output data product.
10. The *creator name* of the output data product must be identical to the *creator name* of the input data product.
11. There must be some additional piece of metadata associated with the output data product that records the translation and what did it.
12. Some translator nodes need to propagate a reference to the original data product for object-ownership purposes.
13. This is needed when the implementation of the output concrete data product makes use of a reference to the input concrete data product, to avoid copying data.
