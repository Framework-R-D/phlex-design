# Phlex Domain Language

Concise shared vocabulary for Phlex design discussions, based on [`doc/appendices/glossary.rst`](doc/appendices/glossary.rst).
This file will be updated periodically, both by the Phlex team and by AI agents, as our domain language matures.
[`doc/appendices/glossary.rst`](doc/appendices/glossary.rst) should be updated as needed.

## Language

### Execution

**Algorithm**:
A user-defined function registered for framework execution, often as the operator of a higher-order function.

**Configured higher-order function (CHOF)**:
An entity formed when an algorithm is registered with its input and output data-product requirements, resource requirements, and higher-order function.

**Computational graph**:
The part of the workflow graph that performs user-facing computations, excluding input and output operations.

**Module**:
A compiled library that the framework can load dynamically.

**Resource**:
A non-data software or hardware component managed by the program for use by algorithms.

### Data Organization

**Data cell**:
A framework-identifiable grouping of data products with a data-layer label and a unique index value within its layer.

**Data layer**:
A group of data cells sharing a data-layer label; the top layer is Job.

**Data-layer hierarchy**:
An acyclic hierarchy of logical containment among data layers, rooted at Job.

**Index set**:
The set of index values that identify members of an indexed family.

**Indexed family**:
A collection whose members are identified by values from an index set.

**APA**:
An anode plane assembly, a physical far-detector unit consisting of thousands of wires arranged as a planar slice.

### Products And Representations

**Product specification**:
The identifying description of a created data product, including a present, non-empty data-layer name.

**Product selection**:
Criteria for matching product specifications. An omitted layer criterion matches any layer; a supplied layer name must be non-empty.

**Physical data product**:
A real-world entity, such as detector readout or a derived physics quantity, that may have multiple concrete representations.

**Data-product concept**:
An abstraction of the computationally relevant aspects of a physical data product; multiple concrete data-product types can model it.

**Concrete data-product type**:
The programming-language type of a concrete data product.

**Concrete data product**:
An object managed by the framework with recorded provenance, produced by an algorithm, provided, or recovered from storage.

**Persistable data product**:
A data product that can be written to and read from persistent storage.

**Translator**:
A component that converts between objects of concrete data-product types modeling the same data-product concept.

**Data model**:
The mechanisms for defining, creating, identifying, organizing, relating, reading, and writing data products.

### Origin And Persistence

**Provenance**:
A description of how data were produced, including information such as product parentage, job configuration, and library versions.

**Algorithmic origin**:
The unique identifier, version, and configuration of the algorithm responsible for producing a particular data product.

**Ancestral data**:
The input data used by the algorithm that produced the data product in question.

**Parentage**:
The set of unique identifiers for each ancestral datum of a data product.

**Configuration canonical form**:
A semantics-preserving representation of a configuration that is identical for all semantically identical configurations.

**Backward compatibility (persisted data)**:
The ability to construct an in-memory representation from information persisted using an earlier in-memory representation.
