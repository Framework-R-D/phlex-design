# Tasks

## Completion And Verification Rules

Framework implementation targets phlex at `../srcs/phlex`. Follow its `AGENTS.md`,
`DEVELOPING.md`, formatting configuration, and `scripts/README.md`; use this change's specification
for behavior and `design.md` for decisions. Preserve the 11 completed entries below verbatim.
They record historical progress, not verification of rebased revision `20059be`.

Current source locations: model registration types/storage are in
`../srcs/phlex/phlex/model/data_product_concept.hpp/.cpp`; typed execution is in
`../srcs/phlex/phlex/core/translator_node.hpp`, with runtime interface/policy in
`declared_translator.hpp/.cpp`. `product_store::stage()` and proxy const/string-view properties
already exist; pending tasks verify/reuse them rather than adding duplicate APIs.

Complete identifier-domain-types R.1-R.3 first; that change alone owns Option A. Then complete
this change's R.1-R.2 before remaining conversion work. Conversion tasks inherit input identity,
record translator identity separately, and use direct-node verification (design Decision 3a).
Catalog identity and automatic placement remain follow-up work.

Each implementation task lands its focused tests and documentation together, using the acceptance
checklists in groups 4 and 5; those groups are not a late testing/documentation phase. Coordinate
shared signature transitions with callers within their task group. Before checking each box, run
`prek run --files <explicit-changed-paths>` or `pre-commit run --files <explicit-changed-paths>`
from the phlex root, inspect hook changes, and rerun until hooks pass. Run the focused full-build
verifier when dependencies permit; otherwise record exactly which test is deferred to the named
task-group gate. Formatting code changes require affected verification to be repeated. Group 6's
formatting sweep is additional integration verification.

Record dated hooks, build, listed/executed tests, and deferrals under each task. Unrun tests are
not passes. Groups 2 and 3 form one coordinated semantic/signature transition, ending at the
common green full-build gate 3.6. The old graph-registration translator expectations cannot pass
after conversion changes until group 3 replaces them. Execute checkpoint 2.9 after that migration,
not as a prerequisite blocking group 3. Dependent test execution may defer only to 3.6, with an
explicit record; tests and documentation still land with their owning edits. Add no temporary
legacy conversion mode and reduce no required selection. Final validation starts after 3.6 passes.
Every gate uses `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '<pattern>'
--verbose` from the initialized workspace, with an anchored pattern naming its tests and fixture
dependencies. The verifier discovers the MPD build directory, selects/builds in the same shell,
lists with `-N`, and executes the same selection with `--no-tests=error`. Missing required tests
prevent completion. Capture full output without truncation pipelines; connect dependent commands
with `&&` and enable `set -o pipefail` in the same invocation for any pipelines. Test-only reruns
require a current build and do not establish freshness. Final validation runs the full suite.

## R. Post-Rebase Baseline Reconciliation

- [ ] R.1 In phlex (`../srcs/phlex`), after identifier-domain-types R.3 passes, audit the completed
  model storage/type-pair tasks and typed single-input/output constraints against current source.
  Confirm `stage()` and proxy const/string-view properties exist and map pending work to the
  current runtime and typed files. Check component access through `name()` and preserve inherited
  product creator separately. Verify the audit against `data_product_concept`, `concepts`, and
  registration/direct-node tests in R.2; record gaps as owned pending work, not historical rewrites.
  Run changed-file hooks if this task changes framework tests or documentation.
- [ ] R.2 In phlex (`../srcs/phlex`), establish a fresh post-rebase baseline with the full-build
  verifier and tests `data_product_concept`, `data_product_concept_registry`, `concepts`,
  `function_registration`, `core_misc`, `transform_node`, `producer_catalog`, `product_selecting`,
  `filter`, and `framework_graph`. Append dated revision/build/listed/executed evidence, preserving
  completed tasks; do not implement conversion semantics merely to make the baseline pass. Route
  failures to their owning task/change and rerun before conversion work.

  Command: `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '^(data_product_concept|data_product_concept_registry|concepts|function_registration|core_misc|transform_node|producer_catalog|product_selecting|filter|framework_graph)$' --verbose`.

## 1. Model-Layer Registration Types And Concept Storage

- [x] 1.1 Add `enum class result_storage { owned, borrowed };` in the model layer, alongside the
      other translator-registration types
- [x] 1.2 Add translator storage to `data_product_concept`, keyed by the ordered pair of input and
      output `concrete_product_id`, holding a record of the translator name, the conversion
      function, and its `result_storage` value
- [x] 1.3 Reject registration when the source and target concrete types are identical (throw)
- [x] 1.4 Reject registration when either type is not already a concrete type of the concept (throw)
- [x] 1.5 Reject registration of a second translator for an already-registered ordered type pair
      (throw)
- [x] 1.6 Add an accessor returning the stored translator record (name, conversion function, and
      `result_storage` value), sufficient for this change's tests

## 2. Core Implementation

- [x] 2.1 Update `is_translator_like` in `phlex/core/concepts.hpp` to require single input and
      single output
- [ ] 2.2 In phlex (`../srcs/phlex/phlex/core/concepts.hpp`), add the distinct-source-and-target
  constraint to `is_translator_like`. Document it and add positive/negative compile-time cases in
  `../srcs/phlex/test/concepts_test.cpp`; verify `concepts` and concept-level identical-type
  rejection in `data_product_concept` with hooks and the full-build verifier at 2.9.
- [x] 2.3 Modify `translator_node` to deduce source type from the function's only parameter and
      target type from its return type
- [x] 2.4 Add static_assert to verify translator_node has exactly one input parameter
- [ ] 2.5 In phlex (`../srcs/phlex/phlex/model/product_store.hpp/.cpp`), reuse and document the
  existing `stage()` accessor. Verify non-default input stages round-trip in
  `../srcs/phlex/test/product_store_test.cpp` and direct-node conversion coverage at 2.9; add no
  duplicate accessor. Run changed-file hooks.
- [ ] 2.6 In phlex (`../srcs/phlex/phlex/core/translator_node.hpp` and
  `declared_translator.hpp/.cpp` as needed), bind the tested input specification for direct
  construction and derive output creator/suffix from it, changing only the concrete type.
  Preserve input index/layer and stage during execution, including empty suffixes and valid absent
  creators where applicable. Keep node `name()` as translator identity and retain existing runtime/
  producer contracts; do not integrate converted nodes into the static catalog. Document the
  direct-node boundary and add numerical/container/callable and identity tests with hooks; verify
  those tests plus `product_store` and existing graph regressions at 2.9. Design: Decisions 2, 3a.
- [ ] 2.7 In phlex (`../srcs/phlex/phlex/model/` product metadata and
  `../srcs/phlex/phlex/core/translator_node.hpp`), record translator identity separately on each
  output product. Obtain node identity through `name()` without replacing inherited creator.
  Document and test both identities, including a qualified translator name, in direct-node and
  product-model coverage; run hooks and verify those CTests at 2.9. Design: Decision 5.
- [ ] 2.8 In phlex (`../srcs/phlex/phlex/model/product_store.hpp/.cpp` and
  `../srcs/phlex/phlex/core/translator_node.hpp`), retain the entire input store for borrowed
  conversions and not for owned ones. Thread `result_storage` into direct construction and
  document whole-store/chained retention. Add lifetime tests with borrowed views and owning
  results, including source-release and chained-store cases; run hooks and verify at 2.9.
  Design: Decision 6.
- [ ] 2.9 Conversion acceptance checkpoint in phlex (`../srcs/phlex`): after 2.2, 2.5-2.8,
  and group 3's coordinated API/regression migration, run a full build and all R.2
  tests plus `product_store`, `products`, and the explicitly registered direct-node conversion
  CTests. Verify conversion, both identities, stages, retention, concurrency, and typed constraints
  at the common 3.6 gate, before final validation. This checkpoint does not block group 3 and
  requires no obsolete graph-registration behavior to survive midway through the transition.
  Record the exact non-empty listed/executed selection and results.

## 3. Registration API

- [ ] 3.1 In phlex (`../srcs/phlex/phlex/core/glue.hpp`), register conversion functions with the
  named concept instead of constructing translator graph registrations. Document registration-only
  behavior and test stored records/no node creation in `../srcs/phlex/test/function_registration_test.cpp`
  and concept coverage. Run hooks and verify at 3.6 with coordinated callers.
- [ ] 3.2 In phlex (`../srcs/phlex/phlex/core/glue.hpp`, `graph_proxy.hpp`, and
  `framework_graph.hpp`), supply translator name/function, concept name, and `result_storage`;
  remove public input-selector/output-suffix chaining. Migrate affected tests/callers and document
  the breaking API. Replace the existing graph-node translator regression deliberately with
  registration-only assertions; preserve conversion execution/metadata coverage in direct-node
  tests from group 2. Verify accepted/rejected registrations and no graph node at 3.6 with hooks.
- [ ] 3.3 In phlex (`../srcs/phlex/phlex/core/graph_proxy.hpp`), retain existing `const` and
  `std::string_view` properties while updating conversion-registration arguments. Document and
  test calls through a const proxy and module-facing interface; run hooks and verify at 3.6.
- [ ] 3.4 In phlex (`../srcs/phlex/phlex/module.hpp`), expose `using base::translate;` for
  `PHLEX_REGISTER_ALGORITHMS`. Add a module-registration test exercising a valid type pair and
  storage declaration, registering its fixture through `../srcs/phlex/test/CMakeLists.txt` as
  needed; document module usage, run hooks, and verify the registered module CTest at 3.6.
- [ ] 3.5 In phlex (`../srcs/phlex/phlex/core/framework_graph.hpp`), delegate `translate()` to
  registration-only concept storage. Document and test its parity with proxy/glue registration,
  complete stored records, and absence of translator graph nodes; run hooks and verify at 3.6.
- [ ] 3.6 Common gate in phlex (`../srcs/phlex`): after both groups 2 and 3, run the full-build
  verifier with every 2.9 test plus
  module-registration coverage after the coordinated API transition. Verify all acceptance cases
  in groups 4 and 5 and record the exact listed/executed selection before final validation.

## 4. Testing Acceptance Checklist

These checks land with their owning group 2 or 3 task. Targets below are in phlex
(`../srcs/phlex/test/`); explicitly register direct-node/module tests in its `CMakeLists.txt`
if existing targets are insufficient. Hooks and the owning gate are required for each check.

- [ ] 4.1 In phlex (`../srcs/phlex/test/function_registration_test.cpp` and
  `data_product_concept_test.cpp`), verify registration creates no node and stored name, callable,
  and storage declaration round-trip. Land with 3.1-3.2; verify at 3.6.
- [ ] 4.2 In phlex (`../srcs/phlex/test/` module fixture and `CMakeLists.txt`), verify registration
  through `PHLEX_REGISTER_ALGORITHMS` and a const proxy. Land with 3.3-3.4; verify at 3.6.
- [ ] 4.3 In phlex (`../srcs/phlex/test/concepts_test.cpp` and `data_product_concept_test.cpp`),
  verify identical-type rejection at compile time and at the concept. Land with 2.2; verify at 2.9.
- [x] 4.4 Test rejection when either type is not a concrete type of the named concept
- [x] 4.5 Test rejection of a duplicate translator for the same ordered type pair
- [ ] 4.6 In phlex (`../srcs/phlex/test/` direct-node coverage), verify int-to-double conversion
  by direct construction, message delivery, and call count. Land with 2.6; verify at 2.9.
- [ ] 4.7 In phlex (`../srcs/phlex/test/` direct-node coverage), verify vector-int-to-vector-double
  conversion by direct construction. Land with 2.6; verify at 2.9.
- [ ] 4.8 In phlex (`../srcs/phlex/test/` direct-node coverage), verify callable-object and lambda
  conversion signatures. Land with 2.6; verify at 2.9.
- [ ] 4.9 In phlex (`../srcs/phlex/test/` direct-node coverage), verify inherited creator/suffix,
  concrete index/layer, and non-default input stage, with node identity different from creator.
  Include empty suffix and valid absent creator; land with 2.5-2.6 and verify at 2.9.
- [ ] 4.10 In phlex (`../srcs/phlex/test/` direct-node/product metadata coverage), verify the
  output records translator identity separately from inherited creator. Land with 2.7; verify at 2.9.
- [ ] 4.11 In phlex (`../srcs/phlex/test/` direct-node/product-store coverage), verify borrowed
  results retain the input store and owned results do not, including chained retention and source
  release. Land with 2.8; verify at 2.9.
- [ ] 4.12 In phlex (`../srcs/phlex/test/` direct-node coverage), verify multiple conversions with
  `concurrency::unlimited`, correct outputs, and counts. Land with 2.6-2.8; verify at 2.9.

## 5. Documentation Acceptance Checklist

- [ ] 5.1 In phlex (`../srcs/phlex/phlex/core/translator_node.hpp` and runtime
  `declared_translator.hpp/.cpp` as needed), document typed execution and direct construction,
  keeping runtime policy separate. Land with 2.6; verify against tests and hooks at 2.9.
- [ ] 5.2 In phlex (`../srcs/phlex/phlex/core/glue.hpp`, `graph_proxy.hpp`, and
  `framework_graph.hpp`), document registration-only `translate()`. Land with group 3; verify
  examples against registration tests and hooks at 3.6.
- [ ] 5.3 In phlex (`../srcs/phlex/phlex/core/translator_node.hpp` and model metadata headers),
  document inherited identity, distinct target type, and separate translator identity. Land with
  2.6-2.7; verify against metadata tests and hooks at 2.9.
- [ ] 5.4 In phlex (`../srcs/phlex/phlex/model/data_product_concept.hpp`, `product_store.hpp`,
  and `../srcs/phlex/phlex/core/translator_node.hpp`), document storage declarations and
  whole-store retention. Land with 2.8; verify against lifetime tests and hooks at 2.9.
- [ ] 5.5 In phlex (`../srcs/phlex/phlex/model/data_product_concept.hpp` and registration headers),
  verify existing/new documentation states concept membership, distinct-type, and duplicate-pair
  invariants. Land with 2.2/group 3; verify against rejection tests and hooks at 3.6.
- [ ] 5.6 In phlex (`../srcs/phlex/phlex/core/glue.hpp` registration documentation), explain
  conversion versus transform usage and deferred automatic placement. Land with group 3; verify
  examples match the public registration API and hooks pass at 3.6.

## 6. Validation

- [ ] 6.1 In phlex (`../srcs/phlex`), run the full-build verifier with every 3.6 test and fixture
  dependency. Verify conversion/registration acceptance checklists are complete, required tests
  exist and pass, and no catalog placement is claimed. Record exact listed/executed evidence.
- [ ] 6.2 In phlex (`../srcs/phlex`), run from the initialized workspace
  `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --verbose` without a test filter.
  Require a full configured MPD build, listing, and full-suite pass; record unavailable optional
  Python/FORM coverage explicitly, without claiming it ran.
- [ ] 6.3 In phlex (`../srcs/phlex`), run `prek run --all-files` or
  `pre-commit run --all-files`, inspect changes, and verify formatting passes. If hooks change
  code, repeat affected verification and the full-suite 6.2 gate before completion.
