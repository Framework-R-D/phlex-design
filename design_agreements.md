# Agreements and Design Decisions

This file records design agreements and agreement-like decisions identified from:

- GitHub issues at [Framework-R-D/phlex issues](https://github.com/Framework-R-D/phlex/issues)
- GitHub organization discussions at [Framework-R-D discussions](https://github.com/orgs/Framework-R-D/discussions)

The agreements are organized below by Phlex subsystem, following the subsystem structure documented in [`phlex-design`](https://github.com/Framework-R-D/phlex-design), especially:

- [Framework Subsystems](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst)
- [Program Startup](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/program_startup.rst)
- [Data Product Management](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/data_product_management.rst)
- [Metadata Management](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/metadata_management.rst)
- [IO](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/io.rst)
- [Resource Management](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/resource_management.rst)
- [Configuration](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/configuration.rst)
- [Algorithm Registration](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/registration.rst)

Some entries are stronger than others:

- Strong: explicit agreement or decision language in the thread
- Partial: meeting notes or converging design direction, but with visible caveats
- Weak/Not included: brainstorming without clear agreement

**This is a snapshot as of 27 May 2026.**

## Task Management

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) as providing the runtime scheduler and executor, workflow-graph types, and computational, provider, translator, preserver, and driver nodes.

### Translator placement and behavior in the graph

Primary sources:

- [#404](https://github.com/Framework-R-D/phlex/issues/404)
- [#535](https://github.com/orgs/Framework-R-D/discussions/535)

Strength: Strong to Partial

Agreements and decisions identified:

From [#404](https://github.com/Framework-R-D/phlex/issues/404):

1. A translator is a node whose input is of one concrete data-product type and whose output is of a different concrete data-product type, both of which model the same data-product concept.
2. A translator will be automatically inserted into the graph between two nodes whose data-product types are concept-compatible but not identical.
3. The computational graph does not include providers, preservers, or the framework driver.
4. The computational graph can be disjoint.
5. Translator nodes belong to the computational graph: there are no translators that precede a provider; there are no translators that follow a preserver.

From [#535](https://github.com/orgs/Framework-R-D/discussions/535):

6. From the flow-graph point of view, translators are treated as first-class nodes, while being mostly irrelevant or invisible to the user.
7. Translator nodes for a given data representation conversion should be singular by construction and should not be duplicated.
8. Translators are not configured directly by the framework user; they must be discovered by the framework based on needs implied by the user configuration.
9. The framework should be able to discover translators and select which ones belong in the graph without loading every discovered translator DLL.
10. The translator metadata required for graph building should be generated at translator plugin build time.

Important caveat from [#535](https://github.com/orgs/Framework-R-D/discussions/535):

- There is explicit disagreement on whether an explicit provider must specify the destination layer for its output products, or whether the layer may be deduced when unambiguous.

### Drivers and runtime hierarchy handling

Primary sources:

- [#395](https://github.com/Framework-R-D/phlex/issues/395)
- [#505](https://github.com/orgs/Framework-R-D/discussions/505)

Strength: Strong

Agreements and decisions identified:

From [#395](https://github.com/Framework-R-D/phlex/issues/395):

1. The hierarchy will be declared to the framework before processing any data-cell indices.
2. Different policies for supporting hierarchies will be supported:
   - a fixed hierarchy may be declared for the entire job
   - the global hierarchy may be updated based on hierarchies stored in input files in the middle of the job
3. Whenever the global hierarchy is established or updated, the framework validates it against what the computational graph requires.
4. Validation may involve creating more index-set nodes, provider nodes, preserver nodes, and translator nodes.
5. The job shall gracefully end when one of the input files contains a hierarchy that does not meet the hierarchy constraints of the computational graph.

From [#505](https://github.com/orgs/Framework-R-D/discussions/505):

6. The semantic distinction between framework drivers and data-product providers will be retained.
7. A data-cell index iterator that supplies one data-cell index at a time is sufficient for v0.3.0.
8. Range-based processing of multiple data cells at one time is not needed for v0.3.0.
9. The data-layer hierarchy will not be known until the input file is opened.
10. Phlex will implement a driver interface that enables opening an input file.
11. After opening the file, FORM will provide metadata used by the framework to synthesize a provider node for each required data product.
12. After v0.3.0, expected future needs include:
   - updating processed hierarchy when opening another input file
   - a mode where external data management declares file hierarchy in advance
   - support for multiple concurrently open input files for multithreading or overlay use cases

## Data Modeling

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) as providing mechanisms by which users define data-product types and the tools used to translate between in-memory representations of data-product concepts.

### Data-product concepts and concrete representations

Primary sources:

- [#404](https://github.com/Framework-R-D/phlex/issues/404)
- [#265](https://github.com/Framework-R-D/phlex/issues/265)

Strength: Strong to Partial

Agreements and notes identified:

From [#404](https://github.com/Framework-R-D/phlex/issues/404):

1. A data-product concept can be modeled by multiple concrete data-product types.
2. A concrete data-product type may model more than one data-product concept.
3. The specification of input data products must support data-product concepts. This may include an optional explicit specification of the data-product concept.

## Data-Product Management

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`data_product_management.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/data_product_management.rst) as covering the organization and identification of data products, data cells, data-product lookup, families, layers, and lifetime management.

### `product_query` semantics and lookup policy

Primary sources:

- [#391](https://github.com/Framework-R-D/phlex/issues/391)
- [#278](https://github.com/Framework-R-D/phlex/issues/278)
- [#506](https://github.com/orgs/Framework-R-D/discussions/506)

Strength: Strong

Agreements and findings identified:

From [#391](https://github.com/Framework-R-D/phlex/issues/391):

1. Only the data-product concept will be unconditionally required, and that could be calculated by the framework.
2. It is a framework-graph initialization error if the `product_query` object resolves to multiple data products.
3. Ambiguities can be resolved by specifying additional fields.
4. This implies that a working Phlex job may fail once a user adds another data product whose specification matches an existing `product_query`.

Open questions noted in [#391](https://github.com/Framework-R-D/phlex/issues/391):

- whether input files should be schema-validated
- whether a summary file or database describing a file set should be supported
- what utilities are needed to inspect available file contents
- what algorithm-configuration options should exist when an expected product is absent
- when input files should be opened during graph formation

From [#278](https://github.com/Framework-R-D/phlex/issues/278):

5. Duplicate specifications, as resolved from `product_query` objects, of the same input data product will be allowed.

From [#506](https://github.com/orgs/Framework-R-D/discussions/506):

6. A `product_query` with an absent layer specification implies that any data products matching the query will be presented to the node.
7. The data layers of data products processed by an algorithm do not necessarily need to be known before forming the data-flow graph.
8. If the fields in the `product_query` are sufficient to unambiguously connect one node to another, the data layer does not need to be known ahead of time.
9. The user must specify a product's data layer, or equivalent disambiguating information using other product-query fields, to guarantee that only products associated with that layer will be processed.

### Data-layer semantics for transforms

Primary source:

- [#398](https://github.com/Framework-R-D/phlex/issues/398)

Strength: Strong

Agreement identified:

1. If a transform algorithm receives input data products from unrelated data layers, the data product created by the algorithm will be associated with every input data layer.

### Identifier restrictions in product specification

Primary source:

- [#343](https://github.com/Framework-R-D/phlex/issues/343)

Strength: Strong

Agreement identified:

1. Phlex will enforce the restriction that the following identifiers used in specifying a data product must be representable as a valid C++ variable name:
   - plugin name
   - node name
   - suffix
   - stage name
   - data-layer name element

## Metadata Management

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`metadata_management.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/metadata_management.rst) as covering the association and retrieval of metadata for jobs, data families, data cells, data products, and related entities.

### Handle-accessible provenance and metadata

Primary sources:

- [#508](https://github.com/orgs/Framework-R-D/discussions/508)
- [#265](https://github.com/Framework-R-D/phlex/issues/265)

Strength: Strong to Partial

Agreements identified about handle behavior from [#508](https://github.com/orgs/Framework-R-D/discussions/508):

1. A data-product handle must provide access to one and only one data product.
2. A constructed handle object is always valid; access to the data product is guaranteed upon successful construction.
3. Handles should not be default-constructible.
4. It is always safe to dereference a handle.
5. It is an error if a handle cannot be constructed when access to a data product is requested.

Agreements identified about data accessible through a handle from [#508](https://github.com/orgs/Framework-R-D/discussions/508):

6. The data product itself should be accessible.
7. The index of the data cell to which the product belongs should be accessible.
8. The data-product concept modeled by the concrete data-product type should be accessible.
9. The creator name should be accessible, including:
   - algorithm name
   - module name in which the algorithm was registered
10. The product suffix should be accessible.
11. The stage name should be accessible.
12. Users should not explicitly invoke anything that creates a handle; the framework creates handles as needed.

## IO

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`io.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/io.rst) as providing mechanisms by which data products and metadata are read and written to persistent storage, through IO plugins and backends such as FORM.

### FORM integration and IO-backed provider synthesis

Primary sources:

- [#505](https://github.com/orgs/Framework-R-D/discussions/505)
- [#404](https://github.com/Framework-R-D/phlex/issues/404)

Strength: Strong

Agreements and decisions identified:

From [#505](https://github.com/orgs/Framework-R-D/discussions/505):

1. Phlex v0.3.0 will support reading one FORM file as input.
2. Phlex will enable registration of providers that return type-erased data.
3. After opening an input file, FORM will provide metadata used by the framework to synthesize a provider node for each required data product.
4. `std::type_info` will be used to request from FORM a data product with a specific type.
5. An unrequited data product will have a provider created for it.
6. For v0.3.0, an exception will be raised if more than one data product in the FORM file matches the specified product query, including its `std::type_info`.
7. Phlex or FORM must be able to generate a reproducible name for a type if no product suffix is associated with the data product.

From [#404](https://github.com/Framework-R-D/phlex/issues/404):

8. The data-product concept must be persisted in some form to a file through Phlex's IO backend interface, such as FORM.

Important caveat from [#505](https://github.com/orgs/Framework-R-D/discussions/505):

- The use of `std::type_info` is called out as potentially problematic across process boundaries. This looks like a `v0.3.0` decision rather than a forever-settled design.

## Resource Management

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`resource_management.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/resource_management.rst) as handling framework program resource requests and runtime resource coordination.

### Replacement for global-state framework services

Primary source:

- [#515](https://github.com/orgs/Framework-R-D/discussions/515)

Strength: Partial

Agreement-like design direction identified:

1. Token-based or resource-limited modeling is the intended direction for replacing framework services that rely on global state.
2. For best parallel performance, functions that require a limited resource should be kept small and fast.
3. In the histogram example discussed, modeling the consuming step as an observer using a limited resource may be preferable to modeling it as a fold over a persistent data product.

This thread reads as converging design exploration rather than a finalized, fully settled interface.

## Framework Application and Program Startup

This area is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and more specifically in [`program_startup.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/program_startup.rst), which covers configuration ingestion, source and module loading, graph assembly, provider matching, translator insertion, and preserver creation.

### Graph assembly and startup-time matching

Primary source:

- [#506](https://github.com/orgs/Framework-R-D/discussions/506)

Strength: Strong

Agreements and findings identified:

1. The program startup design will connect output ports and input ports by an edge when the input-port `product_query` matches the data product of the output port.
2. The framework must be able to detect cycles among computational nodes.
3. It may be easier to form the graph in Python than in C++.
4. Construction of TBB flow-graph nodes may be delayed until graph-formation mechanics are largely complete.
5. User-friendly error messages must be provided for misconfigurations.

Important caveat from [#506](https://github.com/orgs/Framework-R-D/discussions/506):

- The terminology around computational node, pure node, and IO node is explicitly not yet settled.

## Configuration

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`configuration.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/configuration.rst) as handling configuration ingestion, validation, CLI interaction, evaluated configuration semantics, and programmatic access to configuration.

### Configuration-specific discussion status

Reviewed but not counted as a settled agreement:

- [#522](https://github.com/orgs/Framework-R-D/discussions/522)

Assessment:

- No substantive design agreement was recorded in-thread.
- The discussion mostly points back to comments in [#506](https://github.com/orgs/Framework-R-D/discussions/506).

## Registration

This subsystem is described in [`framework_subsystems.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/framework_subsystems.rst) and [`registration.rst`](https://github.com/Framework-R-D/phlex-design/blob/main/doc/ch_subsystem_design/registration.rst) as providing the coupling between user-specified code and the framework, including the registration of algorithms, data-product types, providers, drivers, and plugins.

### Providers and registration policy

Primary sources:

- [#397](https://github.com/Framework-R-D/phlex/issues/397)
- [#505](https://github.com/orgs/Framework-R-D/discussions/505)

Strength: Strong

Agreements identified:

From [#397](https://github.com/Framework-R-D/phlex/issues/397):

1. Providers attached to an input source will be automatically registered for any data products that cannot be provided by an explicit provider or a computational node.
2. Users must be allowed to indicate a precedence for looking up data products from different stages.

From [#505](https://github.com/orgs/Framework-R-D/discussions/505):

3. Phlex will enable registration of providers that return type-erased data.

## Discussions Reviewed But Not Counted As Settled Agreements

### Discussion #519: Data product support

Source: [#519](https://github.com/orgs/Framework-R-D/discussions/519)

Assessment:

- Mostly references to slides and open-ended proposals.
- No clear in-thread consensus or decision text was identified.

### Discussion #594: Asynchronous fanin/fanout

Source: [#594](https://github.com/orgs/Framework-R-D/discussions/594)

Assessment:

- This is a proposal and problem statement from one participant.
- No design agreement or cross-participant consensus was identified.

## Consolidated Remembered Agreement-Bearing Sources

### Issues

- [#404](https://github.com/Framework-R-D/phlex/issues/404)
- [#398](https://github.com/Framework-R-D/phlex/issues/398)
- [#397](https://github.com/Framework-R-D/phlex/issues/397)
- [#395](https://github.com/Framework-R-D/phlex/issues/395)
- [#391](https://github.com/Framework-R-D/phlex/issues/391)
- [#343](https://github.com/Framework-R-D/phlex/issues/343)
- [#278](https://github.com/Framework-R-D/phlex/issues/278)
- [#265](https://github.com/Framework-R-D/phlex/issues/265) as partial

### Discussions

- [#535](https://github.com/orgs/Framework-R-D/discussions/535)
- [#505](https://github.com/orgs/Framework-R-D/discussions/505)
- [#506](https://github.com/orgs/Framework-R-D/discussions/506)
- [#508](https://github.com/orgs/Framework-R-D/discussions/508)
- [#515](https://github.com/orgs/Framework-R-D/discussions/515) as partial

## Contradictions and Tensions in the Current Collection

The current record contains at least one real tension and one apparent contradiction.
There is a third tension that may be simple to resolve but needs to be addressed.

### Data-Product Management: Layer-less `product_query` semantics

Sources:

- [#391](https://github.com/Framework-R-D/phlex/issues/391)
- [#506](https://github.com/orgs/Framework-R-D/discussions/506)

Statements in tension:

From [#391](https://github.com/Framework-R-D/phlex/issues/391):

- It is a framework-graph initialization error if the `product_query` object resolves to multiple data products.

From [#506](https://github.com/orgs/Framework-R-D/discussions/506):

- A `product_query` with an absent layer specification implies that any data products that match the query will be presented to the node.

Why this conflicts:

- One source says multiple matches are an ambiguity error.
- The other says that, at least in the absent-layer case, multiple matches are intentionally accepted and delivered.

Possible reconciliation:

- [#391](https://github.com/Framework-R-D/phlex/issues/391) may apply only in contexts that require exactly one resolved data product.
- [#506](https://github.com/orgs/Framework-R-D/discussions/506) may represent newer or more specific graph-connection semantics.

As written, though, these are contradictory.

### Task Management and Program Startup: Need to know hierarchy or layers before graph formation

Sources:

- [#395](https://github.com/Framework-R-D/phlex/issues/395)
- [#506](https://github.com/orgs/Framework-R-D/discussions/506)

Statements in tension:

From [#395](https://github.com/Framework-R-D/phlex/issues/395):

- The hierarchy will be declared to the framework before processing any data-cell indices.
- The framework validates the hierarchy against graph requirements whenever established or updated.

From [#506](https://github.com/orgs/Framework-R-D/discussions/506):

- Data layers of products processed by an algorithm do not necessarily need to be known before forming the graph.
- If `product_query` fields are otherwise sufficient, the data layer does not need to be known ahead of time.

Why this conflicts:

- One source assumes up-front hierarchy establishment and validation.
- The other says graph formation need not know layers ahead of time.

Possible reconciliation:

- [#395](https://github.com/Framework-R-D/phlex/issues/395) may describe runtime processing hierarchy and validation.
- [#506](https://github.com/orgs/Framework-R-D/discussions/506) may describe minimal information required for edge formation.

Even with that possible reconciliation, the current statements point in different directions and should be treated as a live design tension.

## Non-Contradictions or Compatible Decisions

### IO: `std::type_info` for FORM in v0.3.0

Source: [#505](https://github.com/orgs/Framework-R-D/discussions/505)

Assessment:

- The thread records `std::type_info` as the chosen decision for v0.3.0.
- Concerns about ABI or cross-process boundaries are future-risk notes, not a rejection of the current choice.

### Task Management: Translators as graph entities

Sources:

- [#404](https://github.com/Framework-R-D/phlex/issues/404)
- [#535](https://github.com/orgs/Framework-R-D/discussions/535)

Assessment:

- These are aligned.
- Both sources place translators inside the graph and treat them as graph-level entities.

### Task Management and Data Modeling: Translator auto-insertion and translator discovery

Sources:

- [#404](https://github.com/Framework-R-D/phlex/issues/404)
- [#535](https://github.com/orgs/Framework-R-D/discussions/535)

Assessment:

- These are complementary.
- Auto-insertion addresses graph semantics.
- Discovery without loading all DLLs addresses implementation strategy.

### Metadata Management: Product name to product suffix

Source discussion: [#508](https://github.com/orgs/Framework-R-D/discussions/508)

Related issue context: [#446](https://github.com/Framework-R-D/phlex/issues/446)

Assessment:

- No contradiction has been identified from the material reviewed.
- This looks like normal evolution toward a clearer naming choice.

## Summary of the Most Important Current Tensions

The clearest contradiction in the collected record is:

1. [#391](https://github.com/Framework-R-D/phlex/issues/391): multiple matches are an ambiguity error
2. [#506](https://github.com/orgs/Framework-R-D/discussions/506): absent-layer queries imply all matching products are presented

The next strongest tension is:

1. [#395](https://github.com/Framework-R-D/phlex/issues/395): hierarchy declared and validated up front
2. [#506](https://github.com/orgs/Framework-R-D/discussions/506): layers need not be known before graph formation
