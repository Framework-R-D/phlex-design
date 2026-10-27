# Tasks

## Completion Rules

All implementation work targets phlex at `../srcs/phlex`. Read its `AGENTS.md`
and `DEVELOPING.md`. Use `GLOSSARY.md` for domain terms and the specification
for required behavior. For migration decisions, read `design.md`'s Context and
Decision Index, then only the section named in the task's `Design:` pointer.
Preserve completed task entries; new work remains unchecked until it is done.

Follow the numbered task groups in order. Each implementation task includes its
tests and documentation. Verify the delivered code, tests, and documentation
before checking the box.

Before checking each implementation box, run the existing hooks from the
phlex repository root on every file changed by that task, including unstaged
and new files, naming each path explicitly (the shell does not word-split a
variable): `prek run --files <files>` or `pre-commit run --files <files>`,
whichever is available. Inspect any hook changes and rerun until the hooks
pass. If formatting changes code, repeat the affected build/tests when the
build is green; otherwise defer them to the task group's gate. Task group 9 remains the
integration-wide formatting sweep, not a substitute for these checks.

Run focused tests when dependencies permit. An unrun test is not a passing
test: defer it only to the next gate or checkpoint that will run it. Strict-name
rejection cases run at Checkpoint C.

Record each task's verification as one line under the task:
`Verified (date): hooks pass; built: <targets>; ran: <tests>; deferred: <test> -> <gate or checkpoint>`.

A task group is the numbered set of tasks under one `##` heading. Each of task
groups 3a, 3b, 4, 5, 6, 7, and 8 ends with a gate task: a green full build
(`spack mpd build`, through `verify.sh`) and a passing, targeted set of tests.
The gates are Gate 3a (3a.5), Gate 3b (3b.3), Gate 4 (4.6), Checkpoint A (5.2),
Gate 6 (6.4), Checkpoint B (7.5), and Checkpoint C (8.4). The next task group does
not start until the previous gate passes. A task inside a task group may leave the
build red only for a declared signature transition, and only until the task group's
gate. Do not add compatibility APIs just to compile while a task group is in progress. A transitional
overload that exists only within one task group, and is removed before that task group's
gate, is allowed when it lets a task group land in smaller steps. Gates build the
whole tree, including Python and FORM. If a signature change in the task group breaks
a Python or FORM caller, make the smallest edit there that restores compilation
and record it under the changing task; the later Python and FORM tasks still own
the behavior change, tests, and documentation. Gate procedures follow below.

Task groups 1 and 2 were completed before gates existed, and task group 2's suffix change
(2.4) is not complete until task group 3a migrates its consumers. Verifications
that tasks 1.1-2.6 assign to "Checkpoint A" or "A" run at Gate 3a, as listed in
3a.5. Task group 3 was split into 3a and 3b: old tasks 3.1-3.4 are now 3a.1-3a.4,
old 3.5-3.6 are now 3b.1-3b.2, and 3a.5 and 3b.3 are the gates. Task numbers
inside the verification notes of completed tasks use the new numbers.

## 1. Explicit Empty Values

- [x] 1.1 In phlex (`../srcs/phlex/phlex/model/`), add `empty_string` and
  `identifier_or_empty` construction, text views, and formatting. Document
  the public value states in their headers. Add tests in
  `../srcs/phlex/test/identifier_test.cpp`; verify empty and named text is
  preserved. Run the `identifier` CTest at Checkpoint A.
- [x] 1.2 In phlex (`../srcs/phlex/phlex/model/`), implement equality,
  ordering, and hashing for the values from 1.1. Add tests in
  `../srcs/phlex/test/identifier_test.cpp`; verify equal empty values have
  equal hashes and remain distinct from named values in ordered and
  unordered containers. Run `identifier` at Checkpoint A.
- [x] 1.3 In phlex (`../srcs/phlex/phlex/model/`), define `suffix_t` as the
  sole domain alias of `identifier_or_empty`, with unchanged behavior.
  Document it in the public header and register new headers/sources in
  `../srcs/phlex/phlex/model/CMakeLists.txt`. Verify the installed header
  list and an alias identity check in `identifier_test.cpp`; run at A.
  Leave identifier validation and default-constructor removal for task group 8.

- [x] 1.4 In phlex (`../srcs/phlex/phlex/model/identifier.hpp`), add implicit
  `identifier_or_empty` construction from `char const*` and explicit
  construction from `std::string` and `std::string_view`. Map empty text to
  `empty_string` and named text through `identifier`; retain domain-alternative
  constructors and the valid empty default. Document the lasting hybrid
  policy, including the C-string boundary versus length-bearing inputs.
  Extend `../srcs/phlex/test/identifier_test.cpp` with empty/named construction,
  conversion-trait assertions, and equivalent text, comparison, and hash
  behavior for `identifier_or_empty` and `suffix_t`. Verify at A before
  consuming API migration is accepted; defer invalid-name/embedded-NUL
  rejection to 8.1 at C. Do not reopen completed tasks 1.1-1.3.

  Verification (2026-10-16): `pre-commit run --files phlex/model/identifier.hpp
  test/identifier_test.cpp` passed from the phlex root. The focused MPD build
  and `identifier` CTest passed with 300 assertions in 31 cases. From the workspace
  root, `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir` passed the
  full build and all 119 configured tests, including Python and FORM. Independent
  standards and task-1.4 spec reviews reported no findings. Checkpoint A and
  strict-name/embedded-NUL rejection remain assigned to their later tasks.

## 2. Creators And Product Specifications

Tasks 2.1-2.3 form one creator signature transition. The owning absent-creator
state and direct accessor callers must agree. These are separate work items;
their compilation and tests are verified at Gate 3a (3a.5), because the suffix
transition begun in 2.4 completes in task group 3a. Do not add a temporary empty
`algorithm_name` or compatibility accessor.

- [x] 2.1 In phlex (`../srcs/phlex/phlex/model/algorithm_name.*`), use
  `identifier_or_empty` for the plugin component and `identifier` for the
  algorithm. Remove invalid default creator construction. Accept `find_hits`
  and `reco:find_hits`; reject `:find_hits`, `reco:`, and empty creator text.
  Document parsing and update `../srcs/phlex/test/core_misc_test.cpp`,
  including its default-creator fixture. Verify valid parsing/formatting at A
  and all invalid-name cases at C.
- [x] 2.2 In phlex (`../srcs/phlex/phlex/model/product_specification.*` and
  `../srcs/phlex/phlex/model/products.cpp`), own an optional creator and
  explicitly handle absence in accessors, comparison, hashing, and text.
  Keep `hits`, `/hits`, `""`, and `/` as exact creator-less keys, not wildcards.
  Document this partial label's state: its metadata combines with a concrete
  data-cell/provider layer to form the complete product specification; do not
  add a layer member solely to make this object contain the entire label.
  Add exact-lookup tests in
  `../srcs/phlex/test/product_store_test.cpp` and `products_test.cpp`, plus
  parsing tests in `output_products_test.cpp`. Verify their CTests at A.

  Verification (2026-10-23): `pre-commit run --files` passed from the phlex root on
  the changed files. The `phlex_model` sources and the changed test sources compile,
  and the `products` CTest passed. `product_store`, `output_products`, `core_misc`,
  and `provider` cannot link until `product_selector.cpp` (3a.2) and
  `handle.hpp` (2.5) migrate; they run at Checkpoint A.
- [x] 2.3 In phlex (`../srcs/phlex/phlex/core/consumer.*`,
  `../srcs/phlex/phlex/core/provider_node.cpp`, and
  `../srcs/phlex/phlex/core/detail/make_algorithm_name.cpp`), migrate direct
  creator accessors and initialization, including `consumer::plugin()` returning
  `identifier_or_empty`. Require a creator at provider
  construction. Use an absent plugin only for a null configuration pointer;
  a configured missing/empty `module_label` is an error. Document the
  accessor contract and add cases in `../srcs/phlex/test/core_misc_test.cpp`
  and `provider_test.cpp`. Verify present/absent behavior at A and invalid
  supplied-name cases at C.

  Verification (2026-10-23): `pre-commit run --files` passed from the phlex root on
  the changed files; the changed sources compile. `core_misc` and `provider` run at
  Checkpoint A, once the dependent 2.5 and 3a.2 callers migrate.
- [x] 2.4 In phlex (`../srcs/phlex/phlex/model/product_specification.*` and
  `../srcs/phlex/phlex/core/upstream_predicates.hpp`), store suffixes as
  `suffix_t`. Migrate parsing, defaults, formatting, hashing, and
  `to_product_specifications()` together. Keep fluent suffix registration
  textual; convert empty text to `empty_string` at specification creation.
  Document this boundary and update `../srcs/phlex/test/output_products_test.cpp`,
  including its empty source suffix. Verify default, empty, and named outputs
  at A and invalid named outputs at C.

  Verification (2026-10-23): `pre-commit run --files` passed from the phlex root on
  the changed files. The `phlex_model` sources and `output_products_test.cpp` compile,
  and the `products` CTest passed. `output_products` cannot link until `handle.hpp`
  (2.5) and `product_selector.cpp` (3a.1/3a.2) migrate; it runs at Checkpoint A, and
  invalid named outputs run at C.
- [x] 2.5 In phlex (`../srcs/phlex/phlex/model/handle.hpp`), preserve borrowed
  textual metadata views, with empty creator-component views for absence.
  Update `../srcs/phlex/test/product_handle_test.cpp` and header comments;
  verify empty/named suffixes, absent/present creators, and views backed by
  the owning specification without stored copies. Run `product_handle` at A.

  Verification (2026-10-23): `pre-commit run --files` passed from the phlex root on
  the changed files. `product_handle_test.cpp` compiles against the migrated headers
  (rebuilt after formatting). The registered `product_handle` target cannot link until
  `phlex_core_internal` builds (`product_selector.cpp` and `producer_catalog.cpp`, 3a.1-3a.3),
  so the CTest runs at Checkpoint A. An ad hoc, unregistered build of the same test
  against `phlex_model_internal` passed (12 cases, 56 assertions); it is not a substitute.
- [x] 2.6 In phlex (`../srcs/phlex/phlex/model/product_store.cpp`), replace
  the invalid internal `[Source]` creator with a valid internal name.
  Update expectations in `../srcs/phlex/test/product_store_test.cpp`;
  verify source insertion and exact lookup at A and C.

  Verification (2026-10-23): `pre-commit run --files` passed from the phlex root on
  the changed files. `default_source()` now returns `phlex_source`. The `phlex_model`
  library rebuilt; `product_store_test.cpp` compiles. The registered `product_store`
  target cannot link until `phlex_core_internal` builds (`producer_catalog.cpp`, 3a.3), so
  its CTest runs at Checkpoint A and C. An ad hoc, unregistered build of the
  insertion, creator-less lookup, and new default-source cases against
  `phlex_model_internal` passed the insertion and default-source cases. The 2.2 case
  "creator-less specifications is exact" fails three checks there (`parsed_creatorless`
  has a default `type_id`, so it does not equal or find the `int`-typed specification);
  that case belongs to task 2.2 and must be reconciled before Gate 3a (3a.5).

## 3a. Product Selection, Catalogs, And FORM Writer Text

Tasks 2.4 and 3a.1-3a.3 form one suffix signature transition. Specification
suffixes, selector criteria, catalog keys, and tracked suffix pointers must
agree. FORM writer text conversion in 3a.4 must also land before the build is
green. The files that cannot compile until this transition completes are
`product_selector.cpp` (creator matching, 3a.2), `producer_catalog.cpp` (suffix
keys, 3a.3), `form_module.cpp` (3a.4), and `producer_catalog_test.cpp` (3a.1 and
3a.3). Keep these tasks separate; the task group may leave the build red until Gate 3a
(3a.5), which requires a full build and the tests of task groups 1-3a.

Changing the selector suffix to `std::optional<suffix_t>` (3a.1) is expected to
break readers in the Python plugin (`../srcs/phlex/plugins/python/src/modulewrap.cpp`), which
casts it to a string view and assigns `value_or("")` to an `identifier`. Make the
smallest edit there that restores compilation, using the suffix's text, and
record it under 3a.1. Tasks 6.1-6.3 own the behavior, tests, and documentation.

- [x] 3a.1 In phlex (`../srcs/phlex/phlex/core/product_selector.*`), use
  `std::optional<suffix_t>` for omitted, empty, and named criteria. Document
  and test matching, equality, ordering, and diagnostics in
  `../srcs/phlex/test/product_selector_test.cpp`. Cover designated initializers
  with `"hits"`, `""`, domain alternatives, explicitly constructed runtime
  string values, and an omitted suffix; do not rewrite valid literal calls.
  Replace numeric creator fixtures `1`/`2` with valid names. Verify all three
  states and supported call forms at Gate 3a and C.
  Design: § Preserve selector tri-state semantics at conversion boundaries.

  Verified (2026-10-27): hooks pass; built: `product_selector`, `pymodule`; ran: `product_selector` (116 assertions, 9 cases); deferred: full build -> Gate 3a, strict-name rejection -> C. Python compile repair uses `suffix_t::text()` for intermediate-name text and explicitly constructs the existing provider helper's `identifier` from suffix text; Python boundary behavior remains assigned to task group 6.

- [x] 3a.2 In phlex (`../srcs/phlex/phlex/core/product_selector.*`), retain
  the optional one-name creator criterion that matches a plugin or algorithm.
  Render `[ANY]` as text instead of constructing an identifier in `noexcept`
  formatting. An omitted creator criterion matches a product specification
  with an absent creator; a supplied creator criterion never matches one.
  Document and test omitted, plugin-name, and algorithm-name criteria, and
  both absent-creator cases, in `../srcs/phlex/test/product_selector_test.cpp`.
  Verify at Gate 3a; verify qualified/empty supplied creator rejection at C.
  Design: § Preserve selector tri-state semantics at conversion boundaries.

  Verified (2026-10-27): hooks pass; built: `product_selector`; ran: `product_selector` (116 assertions, 9 cases), including absent-creator matching and textual `[ANY]` diagnostics; deferred: full build -> Gate 3a, qualified/empty supplied creator rejection -> C.

- [x] 3a.3 In phlex (`../srcs/phlex/phlex/core/producer_catalog.*`,
  `../srcs/phlex/phlex/core/producer.hpp`, and
  `../srcs/phlex/phlex/core/node_catalog.cpp`), migrate the current
  `producer_catalog(std::span<producer* const>)` constructor, suffix keys,
  lookup, pointer trackers, and diagnostics together. Retain the independent
  producer interface and collection of transforms, translators, folds, and
  unfolds through `node_catalog::producers()`; do not recreate the removed
  `producing_nodes()` helper. Use display text for `*`, not an identifier.
  Keep plugin components in the creator value. Document the migrated contract
  and extend `../srcs/phlex/test/producer_catalog_test.cpp` with empty/named
  suffixes from an independent producer implementation. Add empty/named/omitted
  lookup and ambiguity assertions in `../srcs/phlex/test/product_selecting_test.cpp`;
  verify `producer_catalog` and `product_selecting` at Gate 3a and C.
  Design: § Migrate the current producer and typed-node architecture.

  Verified (2026-10-27): hooks pass; built: `producer_catalog`, `product_selecting`; ran: `producer_catalog` (26 assertions, 1 case), `product_selecting` (109 assertions, 2 cases); deferred: full build and graph regressions -> Gate 3a, strict-name rejection -> C. Catalog suffix keys and ambiguity trackers use `suffix_t`; output-port pointers and the existing `node_catalog::producers()` collection remain unchanged.

- [x] 3a.4 In phlex (`../srcs/phlex/form/form_module.cpp`), write the suffix
  text from `suffix_t`, without an identifier-only accessor. Update
  `../srcs/phlex/form/README.md` and
  `../srcs/phlex/test/form/form_basics_test.cpp` for empty/named persisted
  text. Verify unchanged text at Gate 3a with `form_basics` and
  `job:form_module`.
  Design: § Keep FORM's persisted text and validate it on conversion.

  Verified (2026-10-27): hooks pass; built: `form_basics`, `form_module`; ran: `form_basics` (287 assertions, 63 cases), including finalized dictionary text and ROOT TTree string-payload readback for empty/named suffixes; deferred: full build and `job:form_module` -> Gate 3a, legacy rejection -> C. An initial integer-payload fixture exposed existing ROOT empty-branch fundamental-payload corruption; the text-preservation regression uses string payloads without changing the storage backend. RNTuple round-trip coverage was not added.

- [x] 3a.5 Gate 3a in phlex (`../srcs/phlex`): after task groups 1-3a, run a
  full build and the tests below. Procedure: § Gate And Checkpoint Procedures.
  The build must be green before task group 3b starts. Tests: `identifier`,
  `products`, `core_misc`, `output_products`, `product_store`, `product_handle`,
  `provider`, `product_selector`, `product_selecting`, `producer_catalog`,
  `form_basics`, and `job:form_module`, plus `py:suffix` and `py:provide` as
  regression checks of existing behavior (new Python behavior belongs to task
  group 6), and `function_registration`, `transform_node`, `multilayer_join`,
  `fold`, and `unfold` as regression checks that the catalog migration preserved
  graph behavior. This gate also runs the tests that tasks 1.1-2.6 deferred to
  "Checkpoint A", which had been blocked because `phlex_core` did not build.
  Before the gate passes, fix the open 2.2 defect: the case "Product store
  lookup of creator-less specifications is exact" in `product_store_test.cpp`
  fails three checks because the parsed specification has a default `type_id`
  while the expected one has an `int` type. Record any Python or FORM compile
  repair made under this task group. Strict-name rejection cases remain deferred
  to C.
  Design: § Migration Plan.

  Gate prerequisite (2026-10-27): the user authorized migrating the empty-suffix helper and output expectation at `../srcs/phlex/test/transform_node_test.cpp:79` and line 131 to `empty_string{}` under 3a.5. The selector's valid literal call remains unchanged. This resolves the fixture dependency previously assigned to 3b.1 without changing graph behavior or weakening this gate; task group 3b otherwise remains untouched.

  Verified (2026-10-27): hooks pass on all 14 changed phlex files; built: full MPD tree, including Python and FORM; ran: all 19 Gate 3a tests through `verify.sh` with the exact listed/executed selection below (19 passed, none unavailable); deferred: strict-name rejection -> C. The 2.2 repair supplies `int` types to parsed creator-less keys and verifies that an untyped key remains distinct. Standards and Spec reviews against phlex `f3fdc9b` and phlex-design `f28e2fd`, repeated after the authorized fixture repair, reported no remaining findings. OpenSpec strict validation passed.

  Gate command from the workspace root: `/opt/homebrew/bin/bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '^(identifier|products|core_misc|output_products|product_store|product_handle|provider|product_selector|product_selecting|producer_catalog|form_basics|job:form_module|py:suffix|py:provide|function_registration|transform_node|multilayer_join|fold|unfold)$'`.

  Full-suite evidence: `/opt/homebrew/bin/bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --verbose` completed the full build and listed/executed all 134 configured CTests; 133 passed and only `scripts:unit` failed. Its two verifier tests found macOS system Bash's empty-array/nounset behavior, also encountered by the initial unqualified `bash` verifier invocation after a successful build. The script-test rerun passed with Homebrew Bash first on `PATH` and the exported Spack function removed from child-shell inheritance; no scripts or application code changed. The reproducible test-only rerun on the current build was `/opt/homebrew/bin/bash -c '. /Users/paterno/spack/share/spack/setup-env.sh && spack mpd select --project phlex-work-dir && spack env activate /Users/paterno/MyProjects/phlex-work-dir/local && export PATH="/opt/homebrew/bin:$PATH" && export -n -f spack && ctest --test-dir /Users/paterno/MyProjects/phlex-work-dir/build -R "^scripts:unit$" -N && ctest --test-dir /Users/paterno/MyProjects/phlex-work-dir/build -R "^scripts:unit$" --output-on-failure --no-tests=error'`.

## 3b. Typed Node Output And Translator Metadata

These tasks build on a green Gate 3a. The typed-node headers and the test
helpers they touch depend on the creator and suffix values from task group 2,
not on the selector and catalog types that 3a migrates, so each task should
keep the build green. Gate 3b (3b.3) confirms it.

- [ ] 3b.1 In phlex (`../srcs/phlex/test/transform_node_test.cpp` and
  `../srcs/phlex/test/multilayer_join_test.cpp`), migrate known empty-suffix
  helpers to explicit empty values without rewriting valid named literals or
  `identifier` arguments preserved by the hybrid policy. Preserve graph behavior;
  verify `transform_node` and `multilayer_join` at Gate 3b and C.
  Design: § Mirror the existing hybrid textual construction policy.
- [ ] 3b.2 In phlex (`../srcs/phlex/phlex/core/transform_node.hpp`,
  `../srcs/phlex/phlex/core/fold_node.hpp`,
  `../srcs/phlex/phlex/core/unfold_node.hpp`, and
  `../srcs/phlex/phlex/core/translator_node.hpp`), migrate typed output
  specification construction to the revised creator/suffix values. Check
  `../srcs/phlex/phlex/core/declared_translator.hpp` and
  `../srcs/phlex/phlex/core/glue.hpp`; preserve the translator's creator,
  registered output suffix, input data-cell index, and execution stage.
  Document that this domain migration does not implement the separate
  translator-type-conversion semantics. In
  `../srcs/phlex/test/function_registration_test.cpp`, rename the stage
  `translator-test` to `translator_test` in registration and expectations.
  Retain creator/suffix/layer/stage assertions and observer consumption of
  catalogued translator output. Verify `function_registration`, `transform_node`,
  `fold`, and `unfold` at Gate 3b and C. If overlapping translator work lands first,
  reconcile against its accepted behavior before migration rather than
  silently importing or reversing that redesign.
  Design: § Preserve current translator metadata during the domain migration; § Migrate the current producer and typed-node architecture.
- [ ] 3b.3 Gate 3b in phlex (`../srcs/phlex`): after task groups 1-3b, run a
  full build and the Gate 3a tests. Procedure: § Gate And Checkpoint Procedures.
  The build must be green before task group 4 starts. The updated tests for
  `function_registration`, `transform_node`, `multilayer_join`, `fold`, and
  `unfold` now run as the verification of 3b.1 and 3b.2, not only as regression
  checks. Strict-name rejection cases remain deferred to C.
  Design: § Migration Plan.

## 4. Data Layers And Provider Registration

Land each task with its callers so the build stays green after it where
practical. If removing a default layer (4.1) or `"*"_id` (4.2) cannot compile
without callers in another task of this task group, add the replacement alongside the
old form, migrate the callers, then remove the old form, all within the task group.
Gate 4 (4.6) requires a full build and passing tests before task group 5 starts.

- [ ] 4.1 In phlex (`../srcs/phlex/phlex/model/data_cell_index.*` and
  `../srcs/phlex/phlex/model/layer_path.*`), remove invalid/default layer
  initialization and require named child layers. Document the rule and
  update nameless-cell cases in `../srcs/phlex/test/data_cell_index_test.cpp`
  and `data_layer_hierarchy_test.cpp`. Verify named paths at Gate 4; verify empty
  and invalid layer rejection in those CTests at C.
  Design: § Separate complete product labels from selection criteria.
- [ ] 4.2 In phlex (`../srcs/phlex/phlex/core/products_consumer.*`), remove
  `"*"_id` for omitted input layers. Prefer collecting only supplied concrete
  layers. Check `declared_transform.cpp`, `declared_observer.cpp`,
  `declared_predicate.cpp`, `declared_translator.cpp`, `declared_fold.cpp`,
  and `declared_unfold.cpp`, plus extracted typed implementations
  `transform_node.hpp`, `observer_node.hpp`, `predicate_node.hpp`,
  `translator_node.hpp`, `fold_node.hpp`, and `unfold_node.hpp` in
  `../srcs/phlex/phlex/core/`. Keep runtime bases and typed nodes separated.
  Single-input omission remains allowed where it is allowed today; multiple
  inputs and folds/unfolds retain required-layer errors. Check
  `multilayer_join_node.hpp` and `fold_join_node.hpp` in
  `../srcs/phlex/phlex/core/` without an unrelated optional-layer rewrite. Document
  and add omission/required-layer assertions in
  `../srcs/phlex/test/product_selecting_test.cpp`; verify it and
  `transform_node`, `multilayer_join`, `fold`, and `unfold` at Gate 4.
  Design: § Separate complete product labels from selection criteria; § Migrate the current producer and typed-node architecture.
- [ ] 4.3 In phlex (`../srcs/phlex/phlex/core/index_router.*`), represent
  unrestricted layer queries without wildcard identifiers. Preserve real
  layer keys and routing behavior. Add omitted/named provider-routing cases
  to `../srcs/phlex/test/provider_test.cpp`; verify `provider` at Gate 4 and C.
  Design: § Separate complete product labels from selection criteria.
- [ ] 4.4 In phlex (`../srcs/phlex/phlex/core/index_router.*`), keep composed
  computational-graph node names in a suitable composed-name key instead of
  converting them to identifiers. Check the caller in
  `../srcs/phlex/phlex/core/make_computational_edges.cpp`; preserve
  `plugin:algorithm` without sanitizing it. Add a configured-creator graph
  case to `../srcs/phlex/test/framework_graph_test.cpp`; verify
  `framework_graph` at Gate 4 and C. This is separate from layer-query routing.
  Design: § Apply domain types to product metadata and creators.
- [ ] 4.5 In phlex (`../srcs/phlex/phlex/core/registration_api.hpp` and
  `../srcs/phlex/phlex/core/provider_node.*`), accept `suffix_t` in explicit
  provider output registration and require a concrete layer and creator.
  Preserve `CURRENT` placeholders, literal empty/named suffix calls, existing
  valid `identifier` arguments, and fluent string registration. Runtime string
  objects require explicit `suffix_t` construction at this typed boundary.
  Document both suffix APIs and their source-compatibility limits; update
  `../srcs/phlex/test/provider_test.cpp` and the explicit empty provider suffix
  in `resources_test.cpp`. Test literal, `empty_string`, named `identifier`,
  and explicitly constructed string/string-view suffix arguments together.
  Verify empty/named suffixes and `CURRENT` at Gate 4; verify invalid required
  names at C.
  Design: § Convert output suffixes at the appropriate registration boundary.
- [ ] 4.6 Gate 4 in phlex (`../srcs/phlex`): after task groups 1-4, run a full build
  and the tests below. Procedure: § Gate And Checkpoint Procedures. The build
  must be green before task group 5 starts. Tests: all Gate 3b tests, plus
  `data_cell_index`, `data_layer_hierarchy`, `layer_path`, `framework_graph`,
  `fold`, `unfold`, `resources`, `job:form_source_coverage_write_ttree`, and
  `job:form_source_coverage_read_ttree` (regression checks of existing FORM
  source behavior). Record any Python or FORM compile repair made under this
  task group. Strict-name rejection cases, including empty data-cell layer names
  (4.1), remain deferred to C.
  Design: § Migration Plan.

## 5. JSON Conversion And Checkpoint A

- [ ] 5.1 In phlex (`../srcs/phlex/phlex/configuration.cpp`), convert absent,
  empty, and named JSON suffix fields to the corresponding selector states.
  Keep supplied creator criteria as one-name identifiers and supplied layers
  and stages as required identifiers. Retain full lengths of supplied strings;
  do not route sized text through the null-terminated constructor. Document this in
  `../srcs/phlex/phlex/configuration.hpp`; add cases to
  `../srcs/phlex/test/config_test.cpp`, including a case showing that a
  supplied empty layer is rejected rather than treated as an omitted layer
  (the selector's `layer_name` already enforces this). Verify the three suffix
  states and the empty-layer rejection at A and invalid/embedded-NUL supplied
  text at C.
  Design: § Preserve selector tri-state semantics at conversion boundaries.
- [ ] 5.2 Checkpoint A in phlex (`../srcs/phlex`): after task groups 1-5, run a full
  build and CTests `identifier`, `core_misc`, `output_products`,
  `product_store`, `products`, `product_handle`, `product_selector`,
  `product_selecting`, `producer_catalog`, `function_registration`,
  `data_cell_index`, `data_layer_hierarchy`,
  `layer_path`, `provider`, `resources`, `transform_node`, `multilayer_join`,
  `framework_graph`, `fold`, `unfold`, and `config`, plus all Gate 4 tests, with
  a non-empty listed/executed selection.
  Verify hybrid constructors, supported provider/selector call forms,
  independent producers, and translator metadata alongside existing behavior.
  Defer only strict-name rejection cases to C; identify
  them explicitly. Python and FORM source behavior migration follows next.
  Procedure: § Gate And Checkpoint Procedures. Design: § Migration Plan.

## 6. Python Conversion

Each task keeps the full build green; 6.2 is an atomic pair. Gate 6 (6.4)
requires a full build and passing Python tests before task group 7 starts.

- [ ] 6.1 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  selector parsing), preserve absent/empty/named suffixes and one-name
  creator criteria. Extract full UTF-8 text with its length and translate
  validation exceptions to Python errors. Document selector behavior in
  `../srcs/phlex/plugins/python/README.md`. Add cases in
  `../srcs/phlex/test/python/suffix.py` and `pysuffix.jsonnet`; verify
  `py:suffix` at Gate 6 and invalid names, qualified selector creators, and
  embedded NUL text at C.
  Design: § Preserve selector tri-state semantics at conversion boundaries.
- [ ] 6.2 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  transform/translator suffix handling), give empty user outputs a valid
  private intermediate suffix instead of `_py`. Update callback registration
  and the returned output-translator selector together; both must name the
  same intermediate product. Keep the final user-visible empty suffix.
  Preserve existing intermediate names for valid named outputs unless migration
  requires a change.
  Document this in `../srcs/phlex/plugins/python/README.md`; extend
  `../srcs/phlex/test/python/suffix.py` and `source.cpp` for empty/named
  products. Verify `py:suffix` end to end at Gate 6 and C. This paired naming
  change stays atomic because either half alone selects the wrong product.
  Design: § Preserve selector tri-state semantics at conversion boundaries.
- [ ] 6.3 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  provider bridge), parse concrete output metadata separately from input
  selectors. Construct qualified/unqualified creators directly, and migrate
  provider suffix helper declarations and calls together to `suffix_t`.
  Translate errors and validate complete text as in 6.1. Document the
  provider contract in `../srcs/phlex/plugins/python/README.md`; extend
  `../srcs/phlex/test/python/pyprovide.py` and `pyprovide.jsonnet` for
  `reco:find_hits`, unqualified creators, and empty/named suffixes. Verify
  `py:provide` at Gate 6 and invalid actual plugin/creator text at C.
  Design: § Preserve selector tri-state semantics at conversion boundaries.
- [ ] 6.4 Gate 6 in phlex (`../srcs/phlex`): after task groups 1-6, run a full build
  and all Checkpoint A tests plus every `^py:` test. Procedure: § Gate And
  Checkpoint Procedures. The build must be green before task group 7 starts. Record
  Python tests that are unavailable in this build. Strict-name rejection cases
  remain deferred to C.
  Design: § Migration Plan.

## 7. FORM Source Conversion And Checkpoint B

Each task keeps the full build green. Verifications "at B" below run at
Checkpoint B (7.5), the gate for this task group.

- [ ] 7.1 In phlex (`../srcs/phlex/form/form_source.cpp`, configuration),
  validate creator, stage, module label, and every supplied plugin/algorithm
  override, even when the existing fallback ignores an incomplete override.
  Preserve stored versus advertised creator names and existing valid fallback
  behavior. Document it in `../srcs/phlex/form/README.md`; extend
  `../srcs/phlex/test/form/form_source_read_ttree.jsonnet` and source
  coverage cases in `../srcs/phlex/test/form/CMakeLists.txt`. Verify valid
  overrides at B and supplied empty/invalid names at C.
  Design: § Keep FORM's persisted text and validate it on conversion.
- [ ] 7.2 In phlex (`../srcs/phlex/form/form_source.cpp`), convert requested
  product suffixes to `suffix_t`; this list is configuration, not dictionary
  discovery. Check text boundaries in
  `../srcs/phlex/form/form/form_reader.cpp` and
  `../srcs/phlex/form/persistence/persistence_reader.cpp` so encountered
  invalid legacy suffix text reports file/product context. Add no dictionary
  enumeration or format changes. Document this limit in
  `../srcs/phlex/form/README.md`; add empty/named round-trip cases and a
  legacy invalid-suffix fixture in `../srcs/phlex/test/form/`, registered in
  its `CMakeLists.txt`. Verify round trips at B and legacy rejection at C.
  Design: § Keep FORM's persisted text and validate it on conversion.
- [ ] 7.3 In phlex (`../srcs/phlex/form/form_source.cpp`, index parsing),
  determine one consistent terminal data-layer name from existing canonical
  indices for the actual stored creator and stage. Treat `[]` as Job.
  Reject missing, malformed, or mixed-layer information; do not use a
  selector layer to resolve it or split hierarchy keys on underscores.
  Add focused cases through `../srcs/phlex/test/form/CMakeLists.txt` for
  Job, repeated cells in one layer, underscore-containing layer names,
  conflicting layers, and missing indices. Verify them at B and C.
  Design: § Keep FORM's persisted text and validate it on conversion.
- [ ] 7.4 In phlex (`../srcs/phlex/form/form_source.cpp`, provider creation),
  use the established layer from 7.3 for matching and provider registration.
  Keep actual creator lookup separate from advertised creator matching.
  Document the accepted index-information limit in
  `../srcs/phlex/form/README.md`; extend source coverage tests in
  `../srcs/phlex/test/form/CMakeLists.txt` for omitted and named selectors,
  non-matching layers, and a supplied layer that cannot resolve mixed or
  missing index information. Verify those cases at B and C.
  Design: § Keep FORM's persisted text and validate it on conversion.
- [ ] 7.5 Checkpoint B in phlex (`../srcs/phlex`): from the workspace root run
  `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '<checkpoint-B-pattern>'`.
  Choose the pattern to include Gate 6's tests (which include A's),
  `py:suffix`, `py:provide`, `form_basics`, `job:form_module`,
  `job:form_source_coverage_write_ttree`,
  `job:form_source_coverage_read_ttree`, and new FORM source cases with their
  fixture dependencies. Verify the full framework, Python plugin, and FORM
  compile before the selected tests execute; use `--verbose` for detailed
  evidence. Verify all migrated behavior passes before task group 8.
  Record optional storage/Python tests that are unavailable in this build.
  Procedure: § Gate And Checkpoint Procedures. Design: § Migration Plan.

## 8. Strict Identifier Validation And Checkpoint C

- [ ] 8.1 In phlex (`../srcs/phlex/phlex/model/identifier.*`), enforce
  non-empty ASCII `^[A-Za-z][A-Za-z0-9_]*$` through every constructor and
  literal, including `_id` and `_idq`. Keep keywords valid. Update public
  invariant comments and `../srcs/phlex/test/identifier_test.cpp`; verify
  valid names retain text, formatting, comparison, and hashes, and empty,
  invalid first characters, punctuation, non-ASCII, and embedded NUL text
  are rejected. Exercise hybrid `identifier_or_empty`/`suffix_t` text paths
  from 1.4: empty text remains empty, invalid non-empty text is rejected rather
  than converted to emptiness, and length-bearing inputs reject embedded NULs
  without truncation. Include empty-capable plugin components while retaining
  non-empty actual plugin names. Run all cases at C.
  Design: § Make `identifier` a validated name; § Mirror the existing hybrid textual construction policy.
- [ ] 8.2 In phlex (`../srcs/phlex/phlex/model/identifier.hpp`), remove
  public default construction. Verify domain owners from earlier task groups
  use explicit valid initialization rather than new sentinel names. Add a
  non-default-constructibility assertion in
  `../srcs/phlex/test/identifier_test.cpp`; retain and test the valid empty
  default of `identifier_or_empty`/`suffix_t`. Verify compilation at C.
  Design: § Make `identifier` a validated name.
- [ ] 8.3 Audit phlex (`../srcs/phlex/phlex/`, `../srcs/phlex/plugins/`,
  `../srcs/phlex/form/`, and `../srcs/phlex/test/`) for default/empty
  identifiers, `"*"_id`, invalid names, and composed text used as identifiers.
  Verify each match is a valid construction or an intentional rejection
  test. Route new fixes to the named owning task and rerun its tests; this
  is an audit, not an unbounded implementation task.
  Design: § Migration Plan.
- [ ] 8.4 Checkpoint C in phlex (`../srcs/phlex`): run
  `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir` from the workspace
  root for a full build, test listing, and full-suite execution without a test
  filter. Verify all required model, graph, registration, JSON, Python, and FORM
  tests pass, including `producer_catalog`, `function_registration`, and every
  deferred rejection case. Record unavailable optional coverage explicitly;
  use `--verbose` for detailed evidence. Verify `adjust_config`
  covers invalid actual stage names and reserved `CURRENT`, while selectors
  and provider registration retain its placeholder behavior.
  Procedure: § Gate And Checkpoint Procedures. Design: § Migration Plan.

## Gate And Checkpoint Procedures

Follow `../srcs/phlex/scripts/README.md`. Empty selections are not successful
verification; record unavailable optional Python/FORM coverage explicitly.

**Gates 3a, 3b, 4, and 6 and Checkpoints A and B** run
`bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '<pattern>'`
from the workspace root, with `<pattern>` an anchored alternation such as
`'^(identifier|products|core_misc)$'` listing the tests the gate names, with
their fixture dependencies. The verifier discovers the build directory, selects
the project and builds the whole tree in the same shell, lists the selection,
and executes the same non-empty selection. Every gate performs a full build,
including Python and FORM; there is no scoped-build exception. A gate may run
more tests than it names, never fewer. Use `--verbose` for detailed evidence.
Checkpoint B's pattern includes A's tests, focused Python/FORM tests, and
their fixture dependencies.

**Checkpoint C** runs `bash srcs/phlex/scripts/verify.sh --project phlex-work-dir`
with no test filter: a full build, test listing, and full-suite execution.

**Every gate and checkpoint** runs fail closed: connect dependent steps with
`&&`, so tests run only after a successful build. Enable `set -o pipefail` in the
same shell invocation when pipelines are used. Capture build and test output
through the tool runner without output-truncation pipelines, and do not parse
output to extract and invoke executables. Test-only reruns require a current
build and do not establish build freshness. If a gate fails, fix the cause in
the task that owns it, or add a task to this task group, and rerun the gate. Do not
start the next task group on a red gate, and do not weaken a gate to pass it.

## 9. Integration Formatting

- [ ] 9.1 In phlex (`../srcs/phlex`), run formatting hooks with
  `prek run --all-files` or `pre-commit run --all-files` as available.
  Verify C++, Python, Jsonnet, and Markdown checks pass and inspect the diff
  for unrelated changes. If formatting changes code, repeat Checkpoint C.
  Design: § Migration Plan.
