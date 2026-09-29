# Tasks

## 1. Identifier And Empty Values

- [ ] 1.1 In phlex (`../srcs/phlex/phlex/model/identifier.*`), validate
  identifier construction, including `_id`, against
  `^[A-Za-z][A-Za-z0-9_]*$`. Update invalid fixtures in
  `../srcs/phlex/test/identifier_test.cpp`; verify its CTest covers empty,
  invalid first characters, punctuation, non-ASCII, keywords, formatting,
  ordering, and hashing.
- [ ] 1.2 In phlex (`../srcs/phlex/phlex/model/`), add `empty_string`,
  `identifier_or_empty`, `suffix_t`, and `plugin_name_t` with consistent text,
  comparison, ordering, formatting, and hash operations. Verify tests in
  `../srcs/phlex/test/identifier_test.cpp` distinguish empty from named values
  in ordered and unordered containers.
- [ ] 1.3 Document the public value invariants and explicit-empty construction
  in phlex model API headers under `../srcs/phlex/phlex/model/`; verify the
  documented constructor behavior against the focused model tests.

## 2. Creator And Product Metadata

- [ ] 2.1 Migrate phlex creator parsing and plugin storage in
  `../srcs/phlex/phlex/model/algorithm_name.*` and
  `../srcs/phlex/phlex/core/detail/make_algorithm_name.cpp` to `plugin_name_t`
  plus a valid algorithm identifier. Move the no-creator/default state into
  `../srcs/phlex/phlex/model/product_specification.*`; verify
  `../srcs/phlex/test/core_misc_test.cpp` and
  `../srcs/phlex/test/output_products_test.cpp` cover qualified, unqualified,
  absent, and invalid creators without an empty `algorithm_name`.
- [ ] 2.2 Migrate phlex `../srcs/phlex/phlex/model/product_specification.*`
  to an explicit `suffix_t`, including legacy suffix-only parsing. Verify
  `../srcs/phlex/test/output_products_test.cpp` covers parsing, formatting,
  empty suffixes, and invalid named suffixes.
- [ ] 2.3 Migrate phlex product-suffix and plugin-name storage in
  `../srcs/phlex/phlex/core/producer_catalog.*` and affected graph metadata
  to their domain aliases. Verify `../srcs/phlex/test/product_selecting_test.cpp`
  still resolves products and reports ambiguous creators or suffixes.
- [ ] 2.4 Document creator and product-specification C++ signatures and the
  absent-creator state in phlex (`../srcs/phlex/phlex/model/algorithm_name.hpp`
  and `../srcs/phlex/phlex/model/product_specification.hpp`); verify the
  documented signatures compile in the focused model tests.

## 3. Concrete Layers And Optional Selection Layers

- [ ] 3.1 In phlex (`../srcs/phlex/phlex/model/data_cell_index.*`), reject
  nameless or invalid child layers and initialize layer paths without default
  identifiers. Update `../srcs/phlex/test/data_cell_index_test.cpp` and
  `../srcs/phlex/test/data_layer_hierarchy_test.cpp`; verify their CTests
  reject `make_child("")` while retaining named-layer paths.
- [ ] 3.2 In phlex (`../srcs/phlex/phlex/core/products_consumer.*`), represent
  omitted input layers without `"*"_id`, retaining required-layer errors and
  any-layer consumers. Verify `../srcs/phlex/test/product_selecting_test.cpp`
  exercises both omitted and specified layer matching.
- [ ] 3.3 In phlex (`../srcs/phlex/phlex/core/index_router.*`), replace
  wildcard identifier keys with explicit absent-layer routing state while
  retaining real layer identifiers. Verify provider-routing CTests under
  `../srcs/phlex/test/` deliver products for unrestricted and named inputs.
- [ ] 3.4 In phlex (`../srcs/phlex/phlex/core/provider_node.*` and
  `../srcs/phlex/phlex/model/data_cell_index.hpp`), require a concrete layer
  when creating or providing a product and document that API requirement.
  Verify `../srcs/phlex/test/provider_test.cpp` rejects empty/invalid output
  layers and preserves valid ones.

## 4. Selector Semantics And Registration

- [ ] 4.1 In phlex (`../srcs/phlex/phlex/core/product_selector.*`), store suffix
  criteria as `std::optional<suffix_t>` and distinguish omitted, empty, and
  named suffixes in matching, equality, ordering, and diagnostics. Verify
  `../srcs/phlex/test/product_selector_test.cpp` covers all three states and
  optional versus named layers.
- [ ] 4.2 In phlex (`../srcs/phlex/phlex/core/producer_catalog.*`), use typed
  suffixes for lookup and replace the `"*"_id` diagnostic with display text.
  Verify `../srcs/phlex/test/product_selecting_test.cpp` retains any-suffix
  lookup and existing ambiguity behavior.
- [ ] 4.3 In phlex (`../srcs/phlex/phlex/core/upstream_predicates.hpp` and
  `../srcs/phlex/phlex/model/product_specification.cpp`), keep fluent
  `output_product_suffixes(...)` textual and convert at specification creation.
  Verify `../srcs/phlex/test/output_products_test.cpp` covers omitted, empty,
  valid named, and invalid named outputs.
- [ ] 4.4 In phlex (`../srcs/phlex/phlex/core/registration_api.hpp`), accept
  `suffix_t` for explicit-provider `output_product(...)` without changing the
  fluent string API or `CURRENT` placeholder. Document both call forms there;
  verify `../srcs/phlex/test/provider_test.cpp` covers empty suffixes, concrete
  layers, and `CURRENT` registration.
- [ ] 4.5 In phlex (`../srcs/phlex/phlex/model/handle.hpp`), keep borrowed
  textual views of plugin, algorithm, suffix, layer, and stage. Verify
  `../srcs/phlex/test/product_handle_test.cpp` covers empty suffixes and view
  lifetimes without adding stored copies.
- [ ] 4.6 Document the optional layer and three suffix-selection states in
  phlex (`../srcs/phlex/phlex/core/product_selector.hpp`); verify
  `../srcs/phlex/test/product_selector_test.cpp` covers every documented case.

## 5. Text And Python Boundaries

- [ ] 5.1 In phlex (`../srcs/phlex/phlex/configuration.cpp`), distinguish
  absent JSON suffixes from `""` and valid names; validate supplied creator,
  layer, and stage text. Document the JSON selector contract in
  `../srcs/phlex/phlex/configuration.hpp`; verify
  `../srcs/phlex/test/config_test.cpp` covers all three suffix states and
  invalid supplied fields.
- [ ] 5.2 In phlex (`../srcs/phlex/plugins/python/src/modulewrap.cpp`),
  distinguish missing Python selector suffixes from empty and named ones,
  validate required fields, and update generated converter suffixes and
  explicit-provider calls to the domain types. Verify integration tests under
  `../srcs/phlex/test/python/` cover all three states and invalid input.
- [ ] 5.3 Document Python selector and registration empty-value behavior and
  the source-breaking typed C++ boundary in phlex
  (`../srcs/phlex/plugins/python/README.md`); verify documented behavior is
  exercised by Python integration CTests under `../srcs/phlex/test/python/`.

## 6. FORM Persistence And Source Layers

- [ ] 6.1 In phlex (`../srcs/phlex/form/form_module.cpp`), serialize `suffix_t`
  as the same persisted text instead of using the identifier-only accessor.
  Verify FORM tests under `../srcs/phlex/test/form/` write empty and named
  suffixes without changing the file format.
- [ ] 6.2 In phlex (`../srcs/phlex/form/form_source.cpp`), convert persisted
  empty suffixes to `empty_string` and named suffixes to validated identifiers;
  reject invalid non-empty legacy values with contextual diagnostics. Verify
  FORM read tests under `../srcs/phlex/test/form/` cover valid round trips and
  an invalid legacy suffix.
- [ ] 6.3 In phlex (`../srcs/phlex/form/form_source.cpp`), obtain each offered
  product's concrete layer from source/product-cell metadata rather than from
  an optional selection layer; report missing concrete metadata clearly.
  Verify FORM source tests under `../srcs/phlex/test/form/` cover omitted and
  named selection layers and the missing-metadata error.
- [ ] 6.4 Document unchanged persisted suffix text, invalid legacy suffix
  rejection, and actual-layer lookup in phlex (`../srcs/phlex/form/README.md`);
  verify FORM read/write CTests under `../srcs/phlex/test/form/` exercise those
  cases.

## 7. Retire Invalid Identifier States

- [ ] 7.1 In phlex (`../srcs/phlex/phlex/`, `../srcs/phlex/plugins/`,
  `../srcs/phlex/test/`), replace remaining invalid default, `""_id`, `"*"_id`,
  and invalid-name fixture uses once their owning domains have migrated.
  Verify a repository-wide search finds no invalid identifier construction
  sites and the affected focused CTests pass.
- [ ] 7.2 In phlex (`../srcs/phlex/phlex/model/identifier.hpp`), remove the
  public default identifier constructor; update value-contract documentation
  there and add a compile-time non-default-constructibility check in
  `../srcs/phlex/test/identifier_test.cpp`. Verify the framework compiles and
  the focused CTest passes.

## 8. Cross-Cutting Integration

- [ ] 8.1 Build phlex (`../srcs/phlex`) with
  `spack mpd select --project phlex-work-dir && spack mpd build`; verify the
  full project, including the Python plugin and FORM, compiles successfully.
- [ ] 8.2 Run phlex (`../srcs/phlex`) formatting hooks using
  `prek run --all-files` or `pre-commit run --all-files` as available; verify
  C++, Python, Jsonnet, and Markdown checks pass without unrelated changes.
- [ ] 8.3 Test phlex (`../srcs/phlex`) with
  `spack mpd select --project phlex-work-dir && spack mpd test -j 12`; verify
  model, registration, selection, Python, and FORM tests pass, including
  rejection of `CURRENT` as an actual execution stage.
