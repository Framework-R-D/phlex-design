## Registries

### The Nature of a Registry

In this context the registry being discussed is an object in the running program.
A data-product concept registry would be owned by the `framework_graph`.
There would be only one registry for data-product concepts in any program.
It would not be a global variable (or singleton) but would be owned by the `framework_graph` and passed to whatever functions need it.
The registry needs to be owned by the `framework_graph` because every time we open a new input file we may discover a need to create new implicit providers, and thus there is the possibility of need new  [translator nodes](the-nature-of-translator-nodes.md).

### The Need for a Registry of Data-Product Concepts

An important reason (perhaps the main reason) for having a registry of data product concepts is related to the need to generate [implicit providers](the-nature-of-implicit-providers.md).
Closely related is the need to generate [translator nodes](the-nature-of-translator-nodes.md) to connect computational nodes.

During graph building, when a calculation node in the graph has an input for which no matching output from another node is found, the graph building system must:
1. Discover the [data-product concept](the-nature-of-data-product-concepts.md) associated with the unmatched input.
2. Determine if the output from another node is a match *except for* the concrete data-product type (if it were also a match for the concrete data-product type, then this would not be an unrequited input!); the graph builder needs to collect the full set of such candidate matches
3. Determine whether there is a [translator function](the-nature-of-translator-functions.md) that can create the unrequited input:
	1. from the output of one of the elements in the set of candidate matches (from above) *or*
	2. by reading from the input file, perhaps followed by the application of a translator function.

Thus the graph-building process must be able to find translators related to a data-product concept.
A registry of data-product concepts can be used to meet this need.

### How Data-Product Concepts Are Added to the Registry

It is not clear whether data-product concepts should be created "early" or "late".
"Early" means during the process of populating the `node_catalog`; as each node is created, it could deal with the concrete data- product types and the data-product concepts that needs.
"Late" means during finalization of the `framework_graph`; the already-created nodes could be traversed, building all of the concept information at one time.

Regardless of whether we handle the processing early or late, we need to do the following for each node:
1. We need to know the concrete data product type and the data-product concept for each input and output.
2. For each of the data-product concepts, we need to check the registry.
	1. If the concept is not in the registry, add it, and add the concrete data product type to the new concept
	2. If the concept is in the registry, and the concrete data product type is not already part of the concept, add it to the concept.
