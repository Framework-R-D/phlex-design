## Why

At the 2026-11-12 source audit of rebased phlex revision `20059be`, the typed `translator_node`
already enforces one input and one output. However, `translate()` still creates a graph producer
whose output uses the translator's creator, registered suffix, and execution stage, rather than
preserving input product identity. Concept-based conversion registration remains separate work.

Users need a way to convert objects from one concrete type to another (e.g.,
`std::vector<int>` → `std::vector<double>`). The existing `transform_node` supports arbitrary
transformations with potentially multiple inputs and outputs, which is more complex than needed
for simple type conversions.

Conversion functions belong to a data-product concept: a concept groups the concrete types that
model it, and a translator function converts between two of those types. A conversion function can
enter the system only by being registered with a concept, so every translator in the system
satisfies two invariants by construction:

- its input and output types model the same data-product concept
- its input and output types are distinct

`translator_node` therefore performs neither check.

This change:

- Modifies `translator_node` to perform one-to-one type conversion, deducing source and target
  types from the conversion function's signature
- Preserves the input product's creator, suffix, layer, and stage on the output product, changing
  only the concrete type
- Records the translator identity on the output product
- Adds translator-function storage to `data_product_concept`, which is where the above invariants
  are enforced
- Turns `translate()` into a registration-only operation: it records a conversion function with a
  named concept and creates no graph node

Registration must supply the translator name, the conversion function, the concept modeled by the
input and output types, and a `result_storage` value stating whether the conversion is *owning*
(its result owns its storage) or *non-owning* (its result refers into the input product's storage).

## What Changes

- Modify `translator_node` to perform one-to-one type conversions
  - Source type deduced from the function's only parameter; target type from its return type
  - Output product preserves the input product's creator, suffix, layer, and stage
  - Output product records the translator identity
  - Output store retains the input store when the conversion is non-owning
- Add translator-function storage to `data_product_concept`, keyed by the input and output
  `concrete_product_id`, which rejects a registration when
  - the source and target types are identical
  - either type is not already a concrete type of the named concept
  - a translator is already registered for that input/output pair
- Change `translate()` on the graph proxy and `framework_graph` to register a conversion function
  with a concept rather than create a node
- Expose `translate` on `module_graph_proxy` so it is reachable from `PHLEX_REGISTER_ALGORITHMS`
- Reuse the existing `product_store::stage()` accessor and add a retained-source-store member
- Update the `is_translator_like` concept to require distinct source and target types, so the
  identical-type error is also caught at compile time

**BREAKING**: `translate()` no longer creates a graph node and no longer accepts input selectors
or output suffixes. It now expects a conversion function with signature
`TargetType convert(SourceType const&)`, a concept name, and a `result_storage` value.

No public API creates a `translator_node` in this change; nodes are created by the graph builder
in a follow-up change. Node behavior is verified by tests that construct `translator_node`
directly.
The existing graph-node registration regression must be updated deliberately when registration
becomes registration-only; its current creator/suffix/stage expectations are not conversion semantics.

## Capabilities

### New Capabilities

- **translator-type-conversion**: Type conversion behavior for `translator_node` and the
  registration of conversion functions with a data-product concept

### Modified Capabilities

None

## Impact

**Affected repositories and guidance:** Shared planning in `phlex-design` follows `AGENTS.md`,
`GLOSSARY.md`, and the translator design context in `doc/ch_conceptual_design/translators.rst`,
`doc/ch_technical_design/data_product_management.rst`, and
`design_wiki/seeds/the-nature-of-translator-nodes.md`. The seed informs the accepted requirements
in this change; automatic placement described by the conceptual design remains a follow-up.
Framework work targets `../srcs/phlex`, following its `AGENTS.md`, `DEVELOPING.md`, and
`scripts/README.md`. No changes to `../srcs/phlex-examples` are planned.

**Overlap:** The immediate Option A consumer repair is owned solely by
`../identifier-domain-types/tasks.md` R.1-R.3. This change uses node component access through
`name()` but keeps inherited product creator metadata separate from translator node identity.
It does not duplicate the repair or import conversion semantics into the identifier migration.

**Affected files:**

- `phlex/core/translator_node.hpp`: typed `translator_node` execution and metadata construction
- `phlex/core/declared_translator.hpp/.cpp`: runtime interface and layer policy
- `phlex/core/concepts.hpp`: `is_translator_like` concept
- `phlex/core/glue.hpp`: `translate()` becomes registration-only
- `phlex/core/graph_proxy.hpp`: `translate()` signature
- `phlex/module.hpp`: expose `translate` on `module_graph_proxy`
- `phlex/core/framework_graph.hpp`: `translate()` registration method
- `phlex/model/data_product_concept.hpp/.cpp`: translator-function storage and its invariants
- `phlex/model/product_store.hpp/.cpp`: reuse `stage()`, add retained source store
- `test/function_registration_test.cpp` and direct-node tests: registration-only and conversion
  verification, replacing obsolete node-registration expectations deliberately

**API changes:**

- Registrants provide conversion functions with explicit source→target signatures, the concept
  name, and a `result_storage` value declaring the conversion owning or non-owning
- Both the source and target types must already be registered as concrete types of the named
  concept before a translator between them can be registered
- Output product inherits creator, suffix, layer, and stage from the input product; callers do not
  specify an output suffix
- Output type is the distinct concrete target type deduced from the conversion function
- Output products record the translator identity

**Memory behavior:**

- Products are owned by their store and cannot be retained individually. When a conversion is
  declared non-owning, the output store retains the **entire** input store, keeping every product
  in it alive for as long as the output lives. Chained non-owning translators retain a chain of
  stores.

**Non-goals:**

- Not changing `transform_node` behavior
- Not modifying the underlying TBB graph execution mechanics
- Not changing the plugin system or module loading
- Not adding the `data_product_concept_registry` interface for looking up translator functions
- Not integrating translator lookup into graph building, and not implementing automatic translator
  discovery or node placement
