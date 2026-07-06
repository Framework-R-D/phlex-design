## The Nature of Translator Functions

The name *translator function* should be preferred to *translator*, to avoid confusion with the term [Translator Node](the-nature-of-translator-nodes.md).
1. A translator function converts an object of one concrete data-product type into an object of a different concrete data-product type that models the same data-product concept.
2. Translators are added to the graph as necessary.
3. A single data-product concept may have multiple concrete representations, such as different language bindings or GPU- and CPU-friendly layouts.
4. A single concrete type may also represent multiple data-product concepts.
5. The framework should be able to discover translator functions and select which ones are needed in [translator nodes](the-nature-of-translator-nodes.md) belong in the graph without loading every discovered translator DLL.
