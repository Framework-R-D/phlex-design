# Tasks

## Completion Rules

All implementation work targets phlex at `../srcs/phlex`. Read its `AGENTS.md`
and `DEVELOPING.md`. Use `GLOSSARY.md` for domain terms and the specification
for required behavior. All boxes remain unchecked until the work is done.

Follow the numbered groups in order. Each implementation task includes its
tests and documentation. Verify the delivered code, tests, and documentation
before checking the box. Run focused tests when dependencies permit. Record
tests that cannot run yet and the checkpoint that will run them; an unrun
test is not a passing test. Run strict-name rejection cases at Checkpoint C.

Before checking each implementation box, run the existing hooks from the
phlex repository root on every file changed by that task, including unstaged
and new files: `prek run --files <files>` or
`pre-commit run --files <files>`, whichever is available. Inspect any hook
changes and rerun until the hooks pass. If formatting changes code, repeat
the affected build/tests when dependencies permit; otherwise record their
checkpoint as above. Group 9 remains the integration-wide formatting sweep,
not a substitute for these changed-file checks.

Checkpoints A, B, and C require successful builds and tests before work
continues. A uses selected framework/test targets in the configured MPD build
directory; obtain it with `spack mpd info --build phlex-work-dir`, not a
hardcoded path. Build with
`cmake --build <build-directory> --target <targets>`. List selected tests with
`ctest --test-dir <build-directory> -N -R <test-pattern>` and confirm that
the selection is not empty. Execute the same selection with
`ctest --test-dir <build-directory> --output-on-failure --no-tests=error -R <test-pattern>`.
For a single named test, anchor its pattern (for example, `'^identifier$'`).

Run verification fail closed: connect dependent steps with `&&`, so tests
run only after a successful build. Enable `set -o pipefail` in the same shell
invocation when pipelines are used. Capture build and test output through
the tool runner without output-truncation pipelines. Use CTest's `-V` option
when detailed evidence is needed, rather than parsing its output to extract
and invoke executables. Keep MPD selection and the dependent build or test
command in the same shell invocation.

## 1. Explicit Empty Values

- [x] 1.1 In phlex (`../srcs/phlex/phlex/model/`), add `empty_string` and
  `identifier_or_empty` construction, text views, and formatting. Document
  the public value states in their headers. Add tests in
  `../srcs/phlex/test/identifier_test.cpp`; verify empty and named text is
  preserved. Run the `identifier` CTest at Checkpoint A.
- [ ] 1.2 In phlex (`../srcs/phlex/phlex/model/`), implement equality,
  ordering, and hashing for the values from 1.1. Add tests in
  `../srcs/phlex/test/identifier_test.cpp`; verify equal empty values have
  equal hashes and remain distinct from named values in ordered and
  unordered containers. Run `identifier` at Checkpoint A.
- [ ] 1.3 In phlex (`../srcs/phlex/phlex/model/`), define `suffix_t` as the
  sole domain alias of `identifier_or_empty`, with unchanged behavior.
  Document it in the public header and register new headers/sources in
  `../srcs/phlex/phlex/model/CMakeLists.txt`. Verify the installed header
  list and an alias identity check in `identifier_test.cpp`; run at A.
  Leave identifier validation and default-constructor removal for group 8.

## 2. Creators And Product Specifications

Tasks 2.1-2.3 form one creator signature transition. The owning absent-creator
state and direct accessor callers must agree. These are separate work items,
but compilation can wait for Checkpoint A. Do not add a temporary empty
`algorithm_name` or compatibility accessor.

- [ ] 2.1 In phlex (`../srcs/phlex/phlex/model/algorithm_name.*`), use
  `identifier_or_empty` for the plugin component and `identifier` for the
  algorithm. Remove invalid default creator construction. Accept `find_hits`
  and `reco:find_hits`; reject `:find_hits`, `reco:`, and empty creator text.
  Document parsing and update `../srcs/phlex/test/core_misc_test.cpp`,
  including its default-creator fixture. Verify valid parsing/formatting at A
  and all invalid-name cases at C.
- [ ] 2.2 In phlex (`../srcs/phlex/phlex/model/product_specification.*` and
  `../srcs/phlex/phlex/model/products.cpp`), own an optional creator and
  explicitly handle absence in accessors, comparison, hashing, and text.
  Keep `hits`, `/hits`, `""`, and `/` as exact creator-less keys, not wildcards.
  Document this state and add exact-lookup tests in
  `../srcs/phlex/test/product_store_test.cpp` and `products_test.cpp`, plus
  parsing tests in `output_products_test.cpp`. Verify their CTests at A.
- [ ] 2.3 In phlex (`../srcs/phlex/phlex/core/consumer.*`,
  `../srcs/phlex/phlex/core/provider_node.cpp`, and
  `../srcs/phlex/phlex/core/detail/make_algorithm_name.cpp`), migrate direct
  creator accessors and initialization. Require a creator at provider
  construction. Use an absent plugin only for a null configuration pointer;
  a configured missing/empty `module_label` is an error. Document the
  accessor contract and add cases in `../srcs/phlex/test/core_misc_test.cpp`
  and `provider_test.cpp`. Verify present/absent behavior at A and invalid
  supplied-name cases at C.
- [ ] 2.4 In phlex (`../srcs/phlex/phlex/model/product_specification.*` and
  `../srcs/phlex/phlex/core/upstream_predicates.hpp`), store suffixes as
  `suffix_t`. Migrate parsing, defaults, formatting, hashing, and
  `to_product_specifications()` together. Keep fluent suffix registration
  textual; convert empty text to `empty_string` at specification creation.
  Document this boundary and update `../srcs/phlex/test/output_products_test.cpp`,
  including its empty source suffix. Verify default, empty, and named outputs
  at A and invalid named outputs at C.
- [ ] 2.5 In phlex (`../srcs/phlex/phlex/model/handle.hpp`), preserve borrowed
  textual metadata views, with empty creator-component views for absence.
  Update `../srcs/phlex/test/product_handle_test.cpp` and header comments;
  verify empty/named suffixes, absent/present creators, and views backed by
  the owning specification without stored copies. Run `product_handle` at A.
- [ ] 2.6 In phlex (`../srcs/phlex/phlex/model/product_store.cpp`), replace
  the invalid internal `[Source]` creator with a valid internal name.
  Update expectations in `../srcs/phlex/test/product_store_test.cpp`;
  verify source insertion and exact lookup at A and C.

## 3. Product Selection And Catalogs

Tasks 2.4 and 3.1-3.3 form one suffix signature transition. Specification
suffixes, selector criteria, catalog keys, and tracked suffix pointers must
agree. FORM writer text conversion in 3.4 must also land before a full build.
Keep these tasks separate, with compilation verified at the named checkpoints.

- [ ] 3.1 In phlex (`../srcs/phlex/phlex/core/product_selector.*`), use
  `std::optional<suffix_t>` for omitted, empty, and named criteria. Document
  and test matching, equality, ordering, and diagnostics in
  `../srcs/phlex/test/product_selector_test.cpp`. Replace numeric creator
  fixtures `1`/`2` with valid names. Verify all three states at A and C.
- [ ] 3.2 In phlex (`../srcs/phlex/phlex/core/product_selector.*`), retain
  the optional one-name creator criterion that matches a plugin or algorithm.
  Render `[ANY]` as text instead of constructing an identifier in `noexcept`
  formatting. Document and test omitted, plugin-name, and algorithm-name
  criteria in `../srcs/phlex/test/product_selector_test.cpp`. Verify at A;
  verify qualified/empty supplied creator rejection at C.
- [ ] 3.3 In phlex (`../srcs/phlex/phlex/core/producer_catalog.*`), migrate
  suffix keys, lookup, `producing_nodes()`, pointer trackers, and diagnostics
  together. Use display text for `*`, not an identifier. Keep plugin
  components in the creator value. Add empty/named/omitted lookup and
  ambiguity assertions to `../srcs/phlex/test/product_selecting_test.cpp`;
  verify the `product_selecting` CTest at A and C.
- [ ] 3.4 In phlex (`../srcs/phlex/form/form_module.cpp`), write the suffix
  text from `suffix_t`, without an identifier-only accessor. Update
  `../srcs/phlex/form/README.md` and
  `../srcs/phlex/test/form/form_basics_test.cpp` for empty/named persisted
  text. Verify unchanged text at B with `form_basics` and `job:form_module`.
- [ ] 3.5 In phlex (`../srcs/phlex/test/transform_node_test.cpp` and
  `../srcs/phlex/test/multilayer_join_test.cpp`), migrate known empty-suffix
  helpers to explicit empty values. Preserve the tested graph behavior;
  verify `transform_node` and `multilayer_join` at A and C.

## 4. Data Layers And Provider Registration

- [ ] 4.1 In phlex (`../srcs/phlex/phlex/model/data_cell_index.*` and
  `../srcs/phlex/phlex/model/layer_path.*`), remove invalid/default layer
  initialization and require named child layers. Document the rule and
  update nameless-cell cases in `../srcs/phlex/test/data_cell_index_test.cpp`
  and `data_layer_hierarchy_test.cpp`. Verify named paths at A; verify empty
  and invalid layer rejection in those CTests at C.
- [ ] 4.2 In phlex (`../srcs/phlex/phlex/core/products_consumer.*`), remove
  `"*"_id` for omitted input layers. Prefer collecting only supplied concrete
  layers. Check `declared_transform.cpp`, `declared_observer.cpp`,
  `declared_predicate.cpp`, `declared_translator.cpp`, `declared_fold.cpp`,
  and `declared_unfold.cpp` in `../srcs/phlex/phlex/core/`.
  Single-input omission remains allowed where it is allowed today; multiple
  inputs and folds/unfolds retain required-layer errors. Check both join
  headers for impact without requiring an optional-layer rewrite. Document
  and add omission/required-layer assertions in
  `../srcs/phlex/test/product_selecting_test.cpp`; verify it and
  `transform_node`, `multilayer_join`, `fold`, and `unfold` at A.
- [ ] 4.3 In phlex (`../srcs/phlex/phlex/core/index_router.*`), represent
  unrestricted layer queries without wildcard identifiers. Preserve real
  layer keys and routing behavior. Add omitted/named provider-routing cases
  to `../srcs/phlex/test/provider_test.cpp`; verify `provider` at A and C.
- [ ] 4.4 In phlex (`../srcs/phlex/phlex/core/index_router.*`), keep composed
  computational-graph node names in a suitable composed-name key instead of
  converting them to identifiers. Check the caller in
  `../srcs/phlex/phlex/core/make_computational_edges.cpp`; preserve
  `plugin:algorithm` without sanitizing it. Add a configured-creator graph
  case to `../srcs/phlex/test/framework_graph_test.cpp`; verify
  `framework_graph` at A and C. This is separate from layer-query routing.
- [ ] 4.5 In phlex (`../srcs/phlex/phlex/core/registration_api.hpp` and
  `../srcs/phlex/phlex/core/provider_node.*`), accept `suffix_t` in explicit
  provider output registration and require a concrete layer and creator.
  Preserve `CURRENT` placeholders and fluent string registration. Document
  both suffix APIs; update `../srcs/phlex/test/provider_test.cpp` and the
  explicit empty provider suffix in `resources_test.cpp`. Verify empty/named
  suffixes and `CURRENT` at A; verify invalid required names at C.

## 5. JSON Conversion And Checkpoint A

- [ ] 5.1 In phlex (`../srcs/phlex/phlex/configuration.cpp`), convert absent,
  empty, and named JSON suffix fields to the corresponding selector states.
  Keep supplied creator criteria as one-name identifiers and supplied layers
  and stages as required identifiers. Document this in
  `../srcs/phlex/phlex/configuration.hpp`; add cases to
  `../srcs/phlex/test/config_test.cpp`. Verify the three suffix states at A
  and invalid/embedded-NUL supplied text at C.
- [ ] 5.2 Checkpoint A in phlex (`../srcs/phlex`): after groups 1-5, build
  targets and run CTests `identifier`, `core_misc`, `output_products`,
  `product_store`, `products`, `product_handle`, `product_selector`,
  `product_selecting`, `data_cell_index`, `data_layer_hierarchy`,
  `layer_path`, `provider`, `resources`, `transform_node`, `multilayer_join`,
  `framework_graph`, `fold`, `unfold`, and `config`. Verify migrated
  behavior passes. Defer only strict-name rejection cases to C; identify
  them explicitly. Python and FORM source target migration follows next.

## 6. Python Conversion

- [ ] 6.1 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  selector parsing), preserve absent/empty/named suffixes and one-name
  creator criteria. Extract full UTF-8 text with its length and translate
  validation exceptions to Python errors. Document selector behavior in
  `../srcs/phlex/plugins/python/README.md`. Add cases in
  `../srcs/phlex/test/python/suffix.py` and `pysuffix.jsonnet`; verify
  `py:suffix` at B and invalid names, qualified selector creators, and
  embedded NUL text at C.
- [ ] 6.2 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  transform/translator suffix handling), give empty user outputs a valid
  private intermediate suffix instead of `_py`. Update callback registration
  and the returned output-translator selector together; both must name the
  same intermediate product. Keep the final user-visible empty suffix.
  Document this in `../srcs/phlex/plugins/python/README.md`; extend
  `../srcs/phlex/test/python/suffix.py` and `source.cpp` for empty/named
  products. Verify `py:suffix` end to end at B and C. This paired naming
  change stays atomic because either half alone selects the wrong product.
- [ ] 6.3 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`,
  provider bridge), parse concrete output metadata separately from input
  selectors. Construct qualified/unqualified creators directly, and migrate
  provider suffix helper declarations and calls together to `suffix_t`.
  Translate errors and validate complete text as in 6.1. Document the
  provider contract in `../srcs/phlex/plugins/python/README.md`; extend
  `../srcs/phlex/test/python/pyprovide.py` and `pyprovide.jsonnet` for
  `reco:find_hits`, unqualified creators, and empty/named suffixes. Verify
  `py:provide` at B and invalid actual plugin/creator text at C.

## 7. FORM Source Conversion And Checkpoint B

- [ ] 7.1 In phlex (`../srcs/phlex/form/form_source.cpp`, configuration),
  validate creator, stage, module label, and every supplied plugin/algorithm
  override, even when the existing fallback ignores an incomplete override.
  Preserve stored versus advertised creator names and existing valid fallback
  behavior. Document it in `../srcs/phlex/form/README.md`; extend
  `../srcs/phlex/test/form/form_source_read_ttree.jsonnet` and source
  coverage cases in `../srcs/phlex/test/form/CMakeLists.txt`. Verify valid
  overrides at B and supplied empty/invalid names at C.
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
- [ ] 7.3 In phlex (`../srcs/phlex/form/form_source.cpp`, index parsing),
  determine one consistent terminal data-layer name from existing canonical
  indices for the actual stored creator and stage. Treat `[]` as Job.
  Reject missing, malformed, or mixed-layer information; do not use a
  selector layer to resolve it or split hierarchy keys on underscores.
  Add focused cases through `../srcs/phlex/test/form/CMakeLists.txt` for
  Job, repeated cells in one layer, underscore-containing layer names,
  conflicting layers, and missing indices. Verify them at B and C.
- [ ] 7.4 In phlex (`../srcs/phlex/form/form_source.cpp`, provider creation),
  use the established layer from 7.3 for matching and provider registration.
  Keep actual creator lookup separate from advertised creator matching.
  Document the accepted index-information limit in
  `../srcs/phlex/form/README.md`; extend source coverage tests in
  `../srcs/phlex/test/form/CMakeLists.txt` for omitted and named selectors,
  non-matching layers, and a supplied layer that cannot resolve mixed or
  missing index information. Verify those cases at B and C.
- [ ] 7.5 Checkpoint B in phlex (`../srcs/phlex`): build with
  `spack mpd select --project phlex-work-dir && spack mpd build`.
  Verify the full framework, Python plugin, and FORM compile. Run A's tests,
  `py:suffix`, `py:provide`, `form_basics`, `job:form_module`,
  `job:form_source_coverage_write_ttree`,
  `job:form_source_coverage_read_ttree`, and new FORM source cases with their
  fixture dependencies. Verify all migrated behavior passes before group 8.
  Record optional storage/Python tests that are unavailable in this build.

## 8. Strict Identifier Validation And Checkpoint C

- [ ] 8.1 In phlex (`../srcs/phlex/phlex/model/identifier.*`), enforce
  non-empty ASCII `^[A-Za-z][A-Za-z0-9_]*$` through every constructor and
  literal, including `_id` and `_idq`. Keep keywords valid. Update public
  invariant comments and `../srcs/phlex/test/identifier_test.cpp`; verify
  valid names retain text, formatting, comparison, and hashes, and empty,
  invalid first characters, punctuation, non-ASCII, and embedded NUL text
  are rejected. Run all cases at C.
- [ ] 8.2 In phlex (`../srcs/phlex/phlex/model/identifier.hpp`), remove
  public default construction. Verify domain owners from earlier groups
  use explicit valid initialization rather than new sentinel names. Add a
  non-default-constructibility assertion in
  `../srcs/phlex/test/identifier_test.cpp`; verify compilation at C.
- [ ] 8.3 Audit phlex (`../srcs/phlex/phlex/`, `../srcs/phlex/plugins/`,
  `../srcs/phlex/form/`, and `../srcs/phlex/test/`) for default/empty
  identifiers, `"*"_id`, invalid names, and composed text used as identifiers.
  Verify each match is a valid construction or an intentional rejection
  test. Route new fixes to the named owning task and rerun its tests; this
  is an audit, not an unbounded implementation task.
- [ ] 8.4 Checkpoint C in phlex (`../srcs/phlex`): run
  `spack mpd select --project phlex-work-dir && spack mpd build` and
  `spack mpd select --project phlex-work-dir && spack mpd test -j 12`.
  Verify all required model, graph, registration, JSON, Python, and FORM
  tests pass, including every deferred rejection case. Verify `adjust_config`
  covers invalid actual stage names and reserved `CURRENT`, while selectors
  and provider registration retain its placeholder behavior.

## 9. Integration Formatting

- [ ] 9.1 In phlex (`../srcs/phlex`), run formatting hooks with
  `prek run --all-files` or `pre-commit run --all-files` as available.
  Verify C++, Python, Jsonnet, and Markdown checks pass and inspect the diff
  for unrelated changes. If formatting changes code, repeat Checkpoint C.
