# Independent Comparison: Option C Designs of 2026-09-17 and 2026-10-30

Provenance: produced 2026-10-30 by a reviewer agent (`explore-high`, high reasoning effort) that took no part in writing either design. Its brief: compare [persistent-references-explorations-2026-09-17_01-result.md](persistent-references-explorations-2026-09-17_01-result.md) ("A") with [persistent-references-explorations-2026-09-30-result.md](persistent-references-explorations-2026-09-30-result.md) ("B") against the shared requirements in [the 2026-09-30 prompt](persistent-references-explorations-2026-09-30-prompt.md) and the art report's Option C section, verifying claims against the framework source and the prototype branch `persistent-references-option-c`, and not sparing the newer design. The reviewer was denied `git` access by policy; its inspection was limited to file reads, so the change set and test results are reported from B, not reproduced.

Disposition of the findings is recorded in the README decision ledger (D13–D15) and in Section 10 of the 10-30 design. Finding 5.1 (binding does not validate element bounds or output type) was confirmed against the code and corrected on 2026-10-30; the test that exercised the defect now asserts rejection. The text below is the reviewer's report, unedited apart from converting absolute paths to repository-relative ones.

Post-review update: the later association-type inconsistency (D16) is also corrected and verified in the uncommitted validation branch; see [the D16 validation record](README.md#7-d16-correction-and-validation-record-association-type-identity). Full CTest remains 137/137 after the correction, while the 81.7 % coverage measurement is a pre-D16 baseline. The review below describes the code at inspection time; its findings and line citations are historical evidence, not a fresh audit of the corrected branch.

---

## Reviewer's report: Option C independent compare/contrast

**Evidence boundary:** I inspected both designs, the brief, the specified comparison-report sections, all four B diagrams, and the framework files listed below. Git inspection was explicitly denied; I could not verify the branch or change set. I did not execute builds or tests. B’s reported test totals and coverage therefore remain reported—not independently reproduced—results. All source links below use absolute paths.

### 1. Core ideas

**A** proposes a comprehensive architecture: registration declares association requirements, a scheduler-invoked resolver supplies handles or `query::many` views, and a global cell registry supplies foreign stores. Algorithms mint output tokens from handles; a YAML schema generator supplies persistence mappings, dictionaries, and evolution machinery. Its strength is treating runtime resolution, output production, storage, and evolution as one lifecycle. However, several interfaces and integration assumptions remain proposals rather than demonstrated mechanisms ([A:55–105][a]).

**B** narrows the initial deliverable to framework-owned associations. Producers combine handle-derived references with local placeholders for their own outputs; consumers declare the association and two target products as graph dependencies. Resolution builds a borrowed pointer view before invocation, and local references are bound after producer invocation. Same-cell and ancestor-cell cases have source-level integration tests; persistence and broader cross-cell lookup are deferred ([reference_resolution.hpp:242–279][rr]; [transform_node.hpp:90–105][tn]; [resolved_association_test.cpp:111–188][gt]).

### 2. Side-by-side comparison

| Dimension | A | B |
| --- | --- | --- |
| Token/identity | Proposed untyped `phlex_ref`: optional cell coordinate, product specification, optional element index. Registry key is layer plus number. | `ref<T>` wraps a bound token or local placeholder. Bound identity includes full hierarchical coordinate, creator/plugin, suffix, stage, optional element index ([reference_token.hpp:34–171][rt]). |
| Minting output references | `ref_to(handle, index)`; does not explain obtaining a handle for an output not yet published. | Actual `ref_to`, `local`, and `local_product`; post-return binding supplies output identity ([reference_token.hpp:177–199][rt]; [transform_node.hpp:94–103][tn]). |
| Consumer declaration | Proposed `.input_association<L,R>(label)` and `.in_cell(...)`. | Actual `resolve<Assoc>(association,left,right)` inside `input_family`; three ports represent one parameter ([reference_resolution.hpp:49–113][rr]; [registration_api.hpp:167–189][ra]). |
| Where/when resolution happens | Scheduler calls a separate resolver before execution. | Retrievers resolve before `std::invoke` enters user code—not a separately scheduled resolver node ([input_arguments.hpp:77–108][ia]; [observer_node.hpp:85–92][on]). |
| Cross-cell cases | General registry lookup proposed; no completed scheduling/lifetime protocol. | Same cell and actual ancestors supported by design/test source. Different sibling cells, descendants, and arbitrary foreign cells are not covered by those tests ([resolved_association_test.cpp:111–188][gt]). |
| Forwarding | Recognizes the need in discussion, but final design supplies no explicit token-preserving consumer API. | Views retain tokens; `ref<T>::forward(token)` preserves identity. Unit tests exercise preservation, but no producer→forwarder→consumer graph appears in the inspected graph tests ([resolved_association.hpp:36–42][rv]; [reference_token_test.cpp:136–141][tt]). |
| Persistence | Generated `phlex_ref`↔FORM token mapping and round-trip tests planned. | Generic logical association layout planned for Stage 1; no association-specific integration found in FORM searches. Existing FORM token is physical file/container/row identity ([token.hpp:20–39][ft]). |
| DSL dependency | YAML generator introduced early; schema evolution central. | Deferred until embedded reference members/generated views. YAML or Jsonnet recommended, not implemented. |
| Verification | Test strategy and acceptance criteria; no demonstrated build. | Catch2 tests and CMake integration exist; 137 passing tests/80.9% coverage are reported by B ([test/CMakeLists.txt:191–205][tc]). |
| Implemented versus described | Searches found none of `phlex_ref`, `CellRegistry`, `query::many`, `mint_ref`, or `input_association` in framework C/C++ files. | Token, association, retriever, binding, and registration APIs exist. Fold/unfold restrictions are explicit ([registration_api.hpp:339–341,478–479][ra]). |
| Brief Q1–Q8 | Q1 output, Q2 staged cross-cell, Q4 schema tooling, Q5 reference varieties, Q7 YAML/evolution, Q8 cell identity addressed conceptually. Q3 topology ownership insufficiently separated from configuration; Q6 forwarding incomplete. | Q1 implemented for associations; Q2 partially implemented/staged; Q3 topology encoded through rows/types, enforcement deferred; Q4/Q7 tooling/evolution deferred; Q5 whole-product and element combinations tested, but standalone reference inputs/reference sets not demonstrated; Q6 token preservation tested; Q8 hierarchical identity implemented ([association.hpp:43–61][ah]; [reference_resolution_test.cpp:253–282][ut]). |

**Numbering clarification:** the brief actually contains **nine** Q/A items. Q9 requires registration-time selection of needed references and completed resolution before execution. Both endorse the timing contract; B implements eager resolution of **every row and both endpoints** of a selected association, not selective endpoint/member resolution ([reference_resolution.hpp:258–277][rr]).

### 3. Agreement: likely robust conclusions

- Persistent identity must be separate from transient access.
- Consumers should not perform storage lookup or resolution themselves.
- Reference-producing algorithms need an output-boundary API even under Option C.
- Whole-product, element, and payload-bearing association cases belong in the design.
- Cross-cell readiness and storage/schema evolution require explicit architectural treatment.
- YAML is a reasonable candidate, but syntax preference alone does not determine the necessary metadata.

These conclusions survive the differences in scope and implementation maturity.

### 4. Disagreements and framework evidence

#### Registry versus declared dependencies

**B has stronger evidence for the initial scope.** Existing edge construction connects declared selectors to producers/providers ([make_computational_edges.cpp:99–115,201–212][edges]). Reusing this machinery gives targets a concrete readiness path.

A’s registry has **no existing cell-owned store to attach to**. `product_store` contains products, an index, source, and stage, but no parent-store or registry access ([product_store.hpp:22–64][ps]). Every transform creates a new store while retaining the selected cell index ([transform_node.hpp:100–105][tn]). Thus one `CellKey → product_store*` entry cannot generally represent all producers’ products for that cell. Index ancestry exists, but is not store ancestry ([data_cell_index.hpp:24–31][dc]).

A registry remains a genuine future option, but needs product/stage discrimination, ownership, publication, retirement, and waiting semantics—not merely a singleton map.

#### Relative versus explicit cell identity

**B’s explicit hierarchy is better supported.** Cell numbers are nested under parents, and its tests distinguish identical terminal APA numbers under different spills ([reference_token_test.cpp:64–71][tt]). A bare layer/number registry key risks collisions unless uniqueness is guaranteed separately.

B’s exact-cell matching **does survive the current repeater**: repeated messages retain the original store and substitute only the message ID ([repeater_node.cpp:87–95,163–169][rep]). Resolution compares against that store’s index, not the child dispatch index ([reference_resolution.hpp:193–199][rr]). This supports ancestor references, not arbitrary sibling lookup.

#### Handle-based minting and consumer views

**A’s handle-based minting is feasible**, despite its simplified handle diagram: real handles expose cell index, creator, suffix, and stage ([handle.hpp:88–98][h]). B correctly uses those surfaces.

A nevertheless omits the not-yet-published output case. B’s local placeholder/binding mechanism addresses that gap. B’s token-bearing pointer rows also solve forwarding more explicitly than A’s proposed handles-only view ([resolved_association.hpp:36–42][rv]).

#### When a DSL is forced

**Deferral is reasonable; mandatory DSL adoption is not established.** B separates runtime association resolution from user-product schema design. But embedded references require traversal, resolved representations, persistence metadata, and evolution rules—not logically a custom language. Handwritten traits/views remain a possible alternative.

Conversely, “one generic persistence layout” is a proposal, not proof of backend readiness. The existing ROOT TBranch reader requires dictionaries/classes for nonfundamental types ([root_tbranch_read_container.cpp:111–124][root]). A is right to expose dictionary generation and evolution as engineering work.

### 5. Weaknesses and risks exposed by the comparison

#### B needs correction or qualification

1. **Binding does not validate referential correctness.** Explicit slots are checked against specification count, but not against the selected output’s type or element bounds. Implicit selection treats scalar `T` and `vector<T>` alike, independently of reference kind ([bind_local_references.hpp:48–105][bind]). A test successfully binds index 2 into a default-empty output vector ([reference_resolution_test.cpp:291–299][ut]). Such errors are deferred to consumption; `fully_bound()` is not a validity guarantee.

2. **“Minted only from received handles” overstates enforcement.** Public `element_token` values and `ref<T>::forward(element_token)` allow arbitrary construction/retyping. This is a necessary forwarding escape hatch, but not capability-enforced provenance ([reference_token.hpp:74–82,149–163][rt]).

3. **Scope is narrower than “covers every requirement.”** Each side resolves against one uniquely selected product/store. Associations spanning multiple target products or sibling cells need more machinery ([reference_resolution.hpp:159–185,256–259][rr]). Q9’s selective-resolution semantics also remain unresolved.

4. **Persistence/evolution are not optional acceptance details.** A better foregrounds round-trip and old-schema testing. B’s runtime prototype does not establish persistent-reference round-tripping.

5. **Borrowed lifetime needs an explicit contract.** Views contain raw source/endpoint pointers, not ownership; retaining or returning them beyond the invocation needs prohibition or support ([resolved_association.hpp:36–44,104–107][rv]).

6. **Verification is narrower than headline coverage.** The graph tests exercise producing transforms and consuming observers, not resolved-input transforms/predicates or forwarding graphs ([resolved_association_test.cpp:111–188][gt]). B’s reported binding coverage is only 56%. Its translator-survival claim is also unverified here: identity preservation alone cannot establish element-index preservation across representations.

#### A needs substantial redesign

- Its registry omits readiness, lifetime, multiple stores per cell, and hierarchical-key problems identified above.
- Mocking or inventing handle/store interfaces risks concealing these integration gaps.
- It does not supply local-output minting or a complete forwarding view.
- “Small startup cost” understates per-invocation resolution.
- Its `.output_association_if` is explicitly graph-construction-time configuration, not data-dependent conditional output.
- Calling fluent registration extensions “macros” does not match the inspected registration architecture ([registration_api.hpp:160–189][ra]; [mock-workflow/algorithm.hpp:75–79][ma]).

### 6. A proposals silently omitted from B’s risks section

These warrant explicit retain/drop/defer decisions:

- Conditional association output and its configuration-versus-data distinction.
- Geometry/logger/calibration service requirements and injection.
- Python/multilanguage reference support.
- Runtime plugin-defined reference fields/schema loading.
- Streaming/dynamic-cell lookup readiness and retention policy.
- Association-link/payload evolution as a distinct compatibility problem.
- Schema-generator regeneration/drift checks in CI.
- A real reconstruction/golden-output integration test.

Schema generation and general evolution were **deferred**, not silently dropped; a global singleton was **explicitly rejected**, not overlooked.

### 7. Recommendation and documentation map

Use **B as the runtime starting point**, after correcting binding validation and narrowing its completeness claims. Carry forward **A’s lifecycle-wide acceptance framing**, especially persistence/evolution, streaming readiness, multilingual support, and realistic integration tests—not its singleton or speculative APIs.

Document three separate contracts:

1. Algorithm-facing minting, forwarding, selective resolution, and borrowed lifetime.
2. Scheduler-facing target scope, readiness, retention, and future gather/registry semantics.
3. Storage-facing identity, round-trip, payload/link evolution, and schema tooling.

Place these in the existing [Data Modeling][modeldoc] and [Data Product Concepts][conceptdoc] documentation map, with explicit implemented/proposed/deferred status.

**Inspection inventory:** Beyond cited files, inspected `reference_token.cpp`, `products.{hpp,cpp}`, `product_specification.hpp`, model `fwd.hpp`, `predicate_node.hpp`, `products_consumer.{hpp,cpp}`, `product_selector.{hpp,cpp}`, `message.hpp`, `multilayer_join_node.hpp`, `detail/repeater_node.hpp`, `index_router.cpp`, model/core CMake files, mock-workflow `types.hpp`, FORM `product_identity.hpp`, `storage_write_association.hpp`, and `persistence_writer.hpp`, under `../../phlex/`. In `../`, inspected the brief, comparison sections, four `diagrams/option-c-*.puml`, conceptual design, and design instructions. A’s transcript was consulted only for forwarding/minting clarification around lines 3207 and 3294–3347.

[a]: persistent-references-explorations-2026-09-17_01-result.md
[rt]: ../../phlex/phlex/model/reference_token.hpp
[rr]: ../../phlex/phlex/core/reference_resolution.hpp
[tn]: ../../phlex/phlex/core/transform_node.hpp
[gt]: ../../phlex/test/resolved_association_test.cpp
[ra]: ../../phlex/phlex/core/registration_api.hpp
[ia]: ../../phlex/phlex/core/input_arguments.hpp
[on]: ../../phlex/phlex/core/observer_node.hpp
[rv]: ../../phlex/phlex/core/resolved_association.hpp
[tt]: ../../phlex/test/reference_token_test.cpp
[ft]: ../../phlex/form/core/token.hpp
[tc]: ../../phlex/test/CMakeLists.txt
[ah]: ../../phlex/phlex/model/association.hpp
[ut]: ../../phlex/test/reference_resolution_test.cpp
[edges]: ../../phlex/phlex/core/make_computational_edges.cpp
[ps]: ../../phlex/phlex/model/product_store.hpp
[dc]: ../../phlex/phlex/model/data_cell_index.hpp
[rep]: ../../phlex/phlex/core/detail/repeater_node.cpp
[h]: ../../phlex/phlex/model/handle.hpp
[root]: ../../phlex/form/root_storage/root_tbranch_read_container.cpp
[bind]: ../../phlex/phlex/core/bind_local_references.hpp
[ma]: ../../phlex/test/mock-workflow/algorithm.hpp
[modeldoc]: ../doc/ch_technical_design/data_modeling.rst
[conceptdoc]: ../doc/ch_technical_design/role_of_data_product_concepts.rst
