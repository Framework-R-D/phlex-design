## Context

See `proposal.md` for motivation and `specs/translator-type-conversion/spec.md` for the conversion
contract. The 2026-11-12 source audit uses rebased phlex revision `20059be`. The typed translator
already enforces one input, one output, and one parameter in
`../srcs/phlex/phlex/core/translator_node.hpp`. Model-level `result_storage`, translator records,
and type-pair invariants exist in `../srcs/phlex/phlex/model/data_product_concept.hpp/.cpp`.
Recorded completed tasks are historical progress, not post-rebase build/test evidence; this audit
ran no framework build or tests. Input-identity inheritance, translator provenance, source-store
retention, distinct-type typed constraints, and registration-only `translate()` remain proposed.

The framework separates runtime interfaces in `declared_*.hpp/.cpp` from typed execution in
`*_node.hpp`. Abstract `consumer` owns input selection, layer policy, and port filtering;
`declared_output` is independent. `declared_translator` currently implements both `consumer` and
the independent `producer` interface, and `node_catalog::producers()` includes translators.
Evidence is in `../srcs/phlex/phlex/core/consumer.hpp`, `declared_output.hpp`,
`declared_translator.hpp/.cpp`, `producer.hpp`, and `node_catalog.cpp`. Current `translate()`
registers a graph node through `glue.hpp`; output metadata uses translator creator/suffix/stage,
while retaining the input data-cell index. This is the behavior the separate identifier migration
preserves, not the conversion behavior specified here.

Shared planning follows `AGENTS.md` and `GLOSSARY.md`; design context is
`doc/ch_conceptual_design/translators.rst`, `doc/ch_technical_design/data_product_management.rst`,
and `design_wiki/seeds/the-nature-of-translator-nodes.md`, reached through `doc/index.rst`.
The seed informs this change's accepted requirements; automatic placement remains a follow-up.
Framework work follows `../srcs/phlex/AGENTS.md`, `DEVELOPING.md`, formatting configuration,
and `scripts/README.md`. No examples-repository implementation is planned.

### Overlap and repair ownership

Option A removes redundant consumer component forwards in favor of `name()` inspection. Its
four-file repair and fresh Gate 3a verification belong solely to
`../identifier-domain-types/tasks.md` R.1-R.3. This change depends on that reconciliation before
resuming implementation and does not duplicate it. The identifier migration preserves current
translator output creator/suffix/stage; this conversion change deliberately replaces those
semantics with inherited input identity and separately recorded translator identity. If either
implementation lands first, reconcile overlapping tests against its accepted behavior without
silently reversing it or merging the changes' contracts.

### Proposed Translator Node Behavior

- Translator nodes convert one input concrete type to a distinct output concrete type
- The conversion function supplies the input and output types through its signature
- The output product preserves the input product's creator, suffix, layer, and stage
- The output product records the translator algorithm identity

### Invariants established elsewhere

A conversion function can enter the system only by being registered with a `data_product_concept`.
That single chokepoint enforces that the source and target types model the same concept and that
they are distinct. `translator_node` consumes functions that already satisfy these invariants and
re-checks neither.

## Goals / Non-Goals

**Goals:**

1. Simplify type conversion registration API for the common use case
2. Deduce types from the function signature to reduce boilerplate
3. Ensure type safety at registration time
4. Preserve product identity through type conversion
5. Maintain compatibility with the TBB graph execution model

**Non-Goals:**

- Do not change `transform_node` behavior
- Do not add the `data_product_concept_registry` interface for looking up translator functions
- Do not integrate translator lookup into graph building; automatic translator discovery and node
  placement are a separate change
- Do not modify the core TBB execution engine
- Do not change plugin loading or the module system

## Decisions

### 1. Type Deduction Strategy

**Decision**: Deduce the source type from the function's only parameter, and the target type from
its return type.

**Rationale**:

- Mirrors user intent: `TargetType convert(SourceType const&)` is the canonical pattern
- Reduces API surface: no need to specify types explicitly
- Leverages existing type deduction infrastructure (`type_deduction.hpp`)

**Alternatives considered**:

- Explicit template parameters: would require users to specify types twice (function signature plus
  template arguments)
- Type traits: rejected. A trait-based indirection would add a customization point that no known
  case needs, since the signature already carries both types unambiguously. Direct deduction
  stands; revisit only if a conversion arises that the signature cannot express.

### 2. Output Product Specification

**Decision**: The output product preserves the input product's metadata:

- Same creator name (from the input product)
- Same suffix (from the input product)
- Same layer and stage (from the input product)
- Concrete type changed to the target type deduced from the function's return type

**Rationale**:

- Type conversion is a 1:1 mapping of the same conceptual data
- The translator is a pass-through for identification metadata
- Only the concrete type changes, not the logical identity
- Once translator nodes are placed by the graph builder, a consumer's selector must be satisfied by
  the translator's output mostly as it would have been by the original producer's; inheriting
  creator, suffix, layer, and stage is what makes the substitution invisible, but with the intended
  difference in the concrete type, which does not match for the original producer but does match for
  the translator.

**Implementation approach**: Construct the output product specification from the input product's
creator and suffix, substituting only the deduced target type; take layer from the input store's
index and stage from the input store.

At the audited revision, `translator_node.hpp` builds output specifications from its own node name
via `to_product_specifications(name(), ...)`, and explicitly passes its captured execution stage
to the output store. Both creator/suffix construction and the source of the stage must change.
Reuse the existing `product_store::stage()` accessor in
`../srcs/phlex/phlex/model/product_store.hpp/.cpp`; do not introduce a duplicate accessor.
Node `name()` identifies the translator. `name().plugin()` and `name().algorithm()` inspect its
components; they do not supply the inherited output product creator. Derive that creator from
the selected input specification, preserving valid absent-creator states where applicable.

**Alternatives considered**:

- Keep the vector API: incorrect for `translator_node`
- Auto-generate a suffix from type names: would be opaque to users

### 3. Registration Is Separate From Node Creation

**Decision**: `translate()` registers a conversion function with a named data-product concept and
creates no graph node.

**Rationale**:

- A conversion function such as `double convert(int const&)` is generic: it applies to any `int`
  product regardless of creator or layer
- Current single-input translators permit omitted selector layers (`require_layers::multi_input_only`
  in `../srcs/phlex/phlex/core/declared_translator.cpp`); mandatory layers are not the rationale
- Node registration binds a conversion to particular product selections and output metadata,
  whereas concept-level type-pair registration is independent of creator, suffix, layer, and stage
- Node placement belongs to the graph builder, which knows which consumer inputs are unsatisfied

**Implementation approach**: `glue::translate` stops calling `make_registration<translator_node>`
and instead records the function with the concept. `framework_graph::translate` follows.
`module_graph_proxy` must add `translate` to its `using base::...` list, and
`graph_proxy::translate` already is `const` and takes `std::string_view`; retain and verify those
properties while updating its registration arguments so it remains callable on the module's
`const&` proxy.

**Consequence**: no public API constructs a `translator_node` in this change. Node behavior is
verified by tests that construct `translator_node` directly against a TBB graph.

### 3a. Direct-Node Construction And Deferred Catalog Integration

**Decision**: This change verifies converted nodes only by direct construction. Construction
binds the input product specification for the tested product and derives the target specification
from its creator and suffix, substituting the target type. Runtime execution preserves that
product identity and the input store's index/stage, while recording the translator's separate
identity. Tests may supply an internal selector to retrieve the bound product; registration-only
`translate()` accepts no input selectors or output suffixes.

**Rationale**: The current static producer catalog reads output suffix/type and uses `node->name()`
as creator before messages execute (`../srcs/phlex/phlex/core/producer_catalog.cpp`). That is not
the inherited product creator contract. Retain runtime/producer interfaces needed by existing nodes,
including declared-node `name()` overrides, and adapt direct construction without cataloguing
converted nodes through that old identity assumption. Do not rename the translator node to the
input creator or change the independent producer interface for unrelated nodes. Registration-only
conversion records create no catalog entry.

**Deferred**: Reconciling catalog product identity with translator identity, translator lookup,
and automatic placement belongs to the graph-builder follow-up. Direct-node tests establish
conversion behavior, not correctness of catalog integration.

**Alternative considered**: Integrate converted nodes into the static catalog in this change.
Rejected because it expands registration-only work into graph placement and would require a
separate catalog identity design. Keeping translator identity as output creator was also rejected
because it violates the accepted conversion contract.

### 4. Concurrency Support

**Decision**: Retain the existing concurrency model (serial/unlimited/custom).

**Rationale**:

- No new execution semantics are needed
- Conversion operations are independent
- Consistent with other node types

### 5. Provenance Metadata

**Decision**: Each output product records the identity of the translator algorithm that produced
it, and nothing more.

**Rationale**:

- Enables tracking of data product lineage through the graph
- Supports FORM I/O and product identification
- Required by translator node semantics (see `design_wiki/seeds/the-nature-of-translator-nodes.md`
  in the separate `phlex-design` repo, items 9 and 11)

**Implementation approach**: Add a translator-identity field to the output product model and
populate it when `translator_node` creates each output product.

**Note**: an earlier version of this decision also recorded a non-owning reference to the input
*product*. That is not representable: a store owns its products by value through `unique_ptr`
elements, so an individual product cannot be referenced or shared across stores. Where a reference
to the source is needed for data validity, it is handled at store granularity by Decision 6.

### 6. Source Retention For Non-Owning Conversions

**Decision**: Registration declares, through a `result_storage` value, whether a conversion is
*owning* (its result owns its storage) or *non-owning* (its result refers to storage owned by the
input product). For a non-owning conversion, the output store retains the input store.

**Rationale**:

- A conversion may legitimately return a view over the source (a span, a pointer-carrying type),
  in which case the result dangles if the source is released
- This cannot be deduced: `std::span` is detectable, but a user-defined type holding a pointer into
  the source is not, so it must be declared
- This is a correctness precondition, not a tuning knob: registering a non-owning conversion as
  owning is a defect the framework cannot detect
- Ordinary owning conversions do not pin stores, so retention costs nothing in the common case

**Naming**: "owning" and "non-owning" describe the result's relationship to its storage, which is
what the registrant actually knows and what determines the lifetime obligation. The earlier term
"aliasing" was avoided because in C++ it is preloaded with type-based alias analysis and strict
aliasing, neither of which is meant here.

**Interface**: the declaration is a two-valued enum, not a `bool`:

```cpp
enum class result_storage { owned, borrowed };
```

A call site reads `result_storage::borrowed` rather than a bare `true`, which matters because
misdeclaring the value is undetectable by the framework. The enumerators name the storage of the
result; `owned` is the ordinary case.

**Implementation approach**: Add a retained-source-store member of type `product_store_const_ptr`
to `product_store`, set by `translator_node` when the conversion was registered non-owning. This
mirrors `unfold_node`, which already holds its parent store alive.

**Granularity**: retention is necessarily whole-store. Products are owned by value inside their
store and only the store is shareable, so retaining the referenced product retains every product in
its store. A chain of non-owning translators retains a chain of stores.

This mechanism is independent of Decision 5: provenance is informational and always present;
retention is about data validity and is conditional.

### 7. Invariant Enforcement At The Concept

**Decision**: `data_product_concept` gains storage for translator functions, keyed by the ordered
pair of input and output `concrete_product_id`. It rejects a registration when:

- the source and target types are identical
- either type is not already a concrete type of that concept
- a translator is already registered for that ordered pair

Rejection throws, matching `register_concept`'s handling of a null concept.

**Rationale**:

- Every translator function passes through this one point, so invariants enforced here hold for the
  whole system and need not be re-checked by `translator_node`
- Rejecting duplicates keeps graph construction deterministic: module load order is not guaranteed,
  so silently choosing between two converters for the same pair would make behavior depend on load
  order
- Throwing aborts module loading rather than silently skipping a registration the author believed
  had taken effect

**Also enforced at compile time**: `is_translator_like` gains a distinct-types constraint, so the
identical-type mistake is caught when registering through the typed interface rather than at module
load. The concept-level check remains for type-erased callers.

### 8. Backward Compatibility

**Decision**: This is a BREAKING change.

**Rationale**:

- The translator API is experimental, but `../srcs/phlex/test/function_registration_test.cpp`
  already exercises graph-node registration, conversion, and translator creator/suffix/stage
- Future users benefit from the simplified API
- Replace the existing node-registration regression deliberately with registration-only assertions
  and direct-node conversion coverage; no claim of zero migration is justified by current source
- Keep Python conversion helpers, which currently register transforms, distinct from translator
  registration (`../srcs/phlex/plugins/python/src/modulewrap.cpp`)

## Risks / Trade-offs

### [Risk] Type deduction might fail for complex signatures

**Mitigation**: Provide clear error messages through compile-time constraints and static
assertions.

### [Risk] Requiring prior type membership makes registration order-dependent

Both converted types must already be concrete types of the named concept, but module load order is
not guaranteed, so a module registering a translator may load before the module that adds those
types to the concept.

**Mitigation**: Report the failure with the concept name and the offending type so the cause is
unambiguous. If ordering proves burdensome in practice, revisit whether translator registration
should add missing types.

### [Risk] Non-owning conversions pin whole stores

Retention is whole-store, so a non-owning translator keeps every product in its input store alive,
and chained non-owning translators keep a chain alive.

**Mitigation**: Confine retention to conversions declared non-owning, and document the granularity
so registrants can prefer owning conversions where practical.

### [Risk] No public creation path in this change

Because registration no longer creates nodes and builder-driven placement is deferred, node
behavior is only exercised by direct construction in tests.

**Mitigation**: Cover conversion, identity inheritance, provenance, and retention by direct
construction so the follow-up change inherits a tested node.

### [Risk] Static catalog identity is mistaken for inherited product identity

The current catalog uses translator node identity as creator; converted products must instead
inherit their input creator.

**Mitigation**: Bind input specifications for direct-node tests and keep converted nodes out of
catalog integration in this change. Decision 3a owns this boundary; the follow-up owns catalog
identity and placement. `name()` component access never authorizes replacing inherited creators.

### [Risk] Completed tasks or existing APIs are mistaken for fresh verification

**Mitigation**: Preserve historical completion and add explicit post-rebase reconciliation and
verification in `tasks.md`. Reuse existing `stage()` and proxy const/string-view properties;
run fresh fail-closed verification rather than adding duplicate interfaces or relying on old gates.

## Migration Plan

Groups 2 and 3 form one coordinated semantic/signature transition: typed conversion changes,
registration-only APIs, and replacement of incompatible old registration expectations must land
together before their common green acceptance gate. The existing `function_registration`
translator case asserts the old catalogued creator/suffix/stage contract and cannot be required
to pass midway through that transition. Tests and documentation accompany their owning edits;
defer dependent execution only to the common gate. Introduce no temporary legacy conversion
mode, reduce no required test selection, and start final validation only after the gate passes.

1. Complete identifier R.1-R.3, then perform this change's pending post-rebase baseline verification.
   Preserve historical completed tasks; confirm existing storage, constraints, and APIs before edits.
2. Complete typed conversion, inherited metadata, provenance, retention, and direct construction in
   `translator_node.hpp` and its runtime base as needed, with their tests and documentation.
3. Change `translate()` to concept registration, preserving existing proxy const/string-view
   properties and adding module exposure. Update registration tests and documentation together.
4. Run the required full build and listed/executed tests under `tasks.md`'s verification procedure.
   Catalog identity and automatic placement remain outside this implementation.

## Open Questions

### How are translator functions represented once the registry hands them to node construction?

`translator_node` is a template whose types are deduced at compile time, but concept storage is
keyed by `std::type_index` and carries no static type information. The follow-up change must decide
whether the concept stores type-erased factories that instantiate the correct
`translator_node<AlgorithmBits>` specialization, or whether `translator_node` becomes a non-template
holding a type-erased callable plus run-time type ids. This choice does not block the present
change, which stores functions but does not look them up.
