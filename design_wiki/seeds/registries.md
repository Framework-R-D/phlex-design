## Registries

### The Need for a Registry of Data-Product Concepts

An important reason (perhaps the main reason) for having a registry of data product concepts it related to the need to generate [implicit providers](the-nature-of-implicit-providers.md).
Closely related is the need to generate [translator nodes](the-nature-of-translator-nodes.md) to connect calculation nodes.

During graph building, when a calculation node in the graph has an input for which no matching output from another node is found, the graph building system must:
1. Discover the [data-product concept](the-nature-of-data-product-concepts.md) associated with the unmatched input.
2. Determine if the output from another node is a match *except for* the concrete data-product type (if it were also a match for the concrete data-product type, then this would not be an unrequited input!); the graph builder needs to collect the full set of such candidate matches
3. Determine whether there is a [translator function](the-nature-of-translator-functions.md) that can create the unrequited input:
	1. from the output of one of the elements in the set of candidate matches (from above) *or*
	2. by reading from the input file, perhaps followed by the application of a translator function.

Thus the graph-building process must be able to find translators related to a data-product concept.
A registry of data-product concepts can be used to meet this need.

### How Data-Product Concepts Are Added to the Registry

1. Every computational node that creates a data product should have a data-product concept associate with the concrete data product type.
2. At the time of the creation of the computational node, the system should add the concept to the registry.
   Note that it is possible that the given concept is already in the registry.
3. At the same time the concrete data product type should be associated with the concept in the registry.
   Note that it is possible that this concrete data product type is already associated with the concept.
