# Design

## Context

`experimental::identifier` currently stores arbitrary strings and is default-constructible, so
the model uses an empty identifier for three different states: a meaningful empty product suffix,
an absent plugin component, and an invalid required name. `algorithm_name`,
`experimental::product_specification`, `product_selector`, configuration conversion, and the Python
plugin bridge consequently each carry or reconstruct those meanings independently.

This design refines the naming model in `proposal.md`, using the product-specification and
product-selector vocabulary in `GLOSSARY.md` and the labeling roles in
`design_wiki/seeds/how-data-products-are-labeled.md`. A complete label for a created product has
a non-empty layer name; a product selector may omit the layer to match any layer. The current
`experimental::product_specification` class contains creator, suffix, and type, while the product's
layer is carried separately (for example, by its data cell or provider). Its name does not imply
that this class alone contains the complete label. Follow `AGENTS.md` for shared design and
`../srcs/phlex/AGENTS.md` and `../srcs/phlex/DEVELOPING.md` for framework migration.

The audited framework baseline is revision `74e8e6c`. It already defines `empty_string`,
`identifier_or_empty`, and the `suffix_t` alias in `../srcs/phlex/phlex/model/identifier.hpp`,
but suffix-bearing APIs still use `identifier`, which still accepts invalid and empty names.
`identifier_or_empty` currently accepts its domain alternatives, not text directly. The textual
construction policy below is proposed migration work, not behavior already implemented by those
types. Existing `identifier` construction is implicit from `char const*` and explicit from
`std::string` and `std::string_view`; preserve that distinction for empty-capable values.

That baseline separates runtime node bases from typed implementations and constructs the producer
catalog through an independent `producer` interface. Translators now participate in that catalog
and produce stage-aware products. These are implementation facts verified in
`../srcs/phlex/phlex/core/producer.hpp`, `node_catalog.cpp`, `producer_catalog.cpp`, and
`translator_node.hpp`, rather than requirements imported from the separate
`../translator-type-conversion/proposal.md`.

The architecture context is `doc/ch_technical_design/data_product_management.rst` and
`doc/ch_technical_design/registration.rst`, reached through `doc/index.rst` and
`doc/supporting_design.rst`. The labeling note above is a design seed; use this change's
specification for its required naming behavior. Repository-specific formatting and verification
remain governed by `../srcs/phlex/AGENTS.md`, `../srcs/phlex/.clang-format`,
`../srcs/phlex/.clang-tidy`, `../srcs/phlex/.pre-commit-config.yaml`, and
`../srcs/phlex/scripts/README.md`.

## Goals / Non-Goals

**Goals:**

- Make construction of a generic `identifier` enforce the agreed ASCII syntax, so required names
  are valid by construction.
- Represent a semantically meaningful empty string explicitly and make empty-or-named values
  usable anywhere suffixes and plugin components are stored, compared, formatted, or hashed.
- Keep actual plugin names as non-empty `identifier` values; permit emptiness only in components
  whose owning domain allows the plugin name to be absent.
- Preserve three selector states: omitted suffix (`std::nullopt`), explicit empty suffix, and
  explicit non-empty suffix.
- Keep a product selector's omitted layer distinct from a concrete, non-empty layer name.
- Keep C++, JSON configuration, and Python conversion at clear boundaries that construct the
  correct domain type from textual input.
- Preserve convenient literal construction at typed suffix boundaries while requiring explicit
  construction from runtime string objects; both paths must produce the same domain values.

**Non-Goals:**

- Introduce a `plugin_name_t` alias or distinct runtime validation or behavior for `suffix_t`;
  `suffix_t` remains an alias of `identifier_or_empty` with the same value semantics.
- Change the existing creator matching or product-selector rules beyond making their empty and
  omitted inputs explicit.
- Change the handle's textual metadata views, the fluent string suffix API, the FORM file format,
  or the stage-specific reservation of `CURRENT`.
- Redesign the data-layer hierarchy or stage and algorithm naming beyond applying the generic
  identifier syntax to their names.
- Introduce the separate translator-type-conversion work.

## Decisions

### Make `identifier` a validated name

`experimental::identifier` will retain its string storage, precomputed hash, equality, ordering,
formatting, hashing, and `_id` literal interface. Its construction paths, including the literal
and conversions from configuration or Python, will accept only non-empty ASCII text matching
`^[A-Za-z][A-Za-z0-9_]*$`. Both letter cases are allowed; digits and underscores may follow the
first letter. C++ keywords (for example, `class`) are allowed. The type will no longer have a
public invalid/default-constructed state. Owning types that rely on default construction must
use domain-valid initialization instead.

This places the syntax invariant at the value boundary instead of relying on scattered checks
such as those in `creator_name` and `layer_name`. The explicit ASCII rule supersedes the older
"valid C++ variable name" wording in `design_agreements.md`, which would exclude C++ keywords.
`CURRENT` passes generic validation but remains reserved for an actual execution stage; its
placeholder uses in selections and registration are still valid
(`../srcs/phlex/phlex/app/run.cpp`, `../srcs/phlex/phlex/core/registration_api.hpp`).

Alternative considered: retain empty `identifier` and add validation at every API that requires a
name. Rejected because APIs already disagree about emptiness and future call sites could silently
reintroduce invalid states.

### Separate complete product labels from selection criteria

A created product's complete specification must carry a concrete, non-empty layer name, modeled
by `identifier` at the layer-creation and provider boundaries. It is not necessary to add a layer
member to `experimental::product_specification`: its existing creator/suffix/type metadata combines
with a data-cell or provider layer to form the complete label. A product selector's layer
criterion remains optional: omitted matches any concrete layer, while a supplied name is a valid
`identifier`; an empty string is neither a valid layer name nor a wildcard. This distinction
matches `GLOSSARY.md` and the matching API in `../srcs/phlex/phlex/core/product_selector.hpp`.

`data_cell_index::make_child("")` and its nameless-cell tests must cease to succeed. The existing
`"*"_id` sentinels for omitted layers or suffixes in the consumer, router, and producer catalog
must become distinct wildcard/query state or display text rather than invalid `identifier` values;
preserve the existing any-layer and any-suffix selection behavior.

Alternative considered: encode an omitted product-selector layer with an empty identifier or `"*"`.
Rejected because neither is a valid name, and a wildcard is not a concrete layer.

### Model meaningful emptiness as a sum value

Introduce `empty_string` as the sole explicit marker for an empty textual value, and introduce
`identifier_or_empty` as the value type containing either `empty_string` or a valid `identifier`.
It will expose the common value operations required by existing suffix and plugin consumers:
comparison, ordering, conversion/formatting to text, and hashing. Hash and comparison semantics
must distinguish the empty alternative from every non-empty identifier while retaining stable
value behavior for maps and unordered containers.

Define `suffix_t` as an alias of `identifier_or_empty`, with no additional validity rules or
behavior. Use `suffix_t` for product suffixes and `identifier_or_empty` directly for plugin
components that may be empty. Actual plugin names remain `identifier`; no `plugin_name_t` alias
is introduced. This keeps the empty-capable component distinct from the non-empty name it may
contain without introducing another type name with identical value semantics.

Alternative considered: use `std::optional<identifier>` for meaningful emptiness. Rejected because
an optional already means an omitted product-selector field; using it for both would collapse explicit
empty and unspecified suffixes. A raw `std::string` was also rejected because it would weaken the
validity guarantee of the non-empty alternative.

### Mirror the existing hybrid textual construction policy

Add implicit construction of `identifier_or_empty` from `char const*`, and explicit construction
from `std::string` and `std::string_view`. This applies equally to `suffix_t`, because it is an
alias, and to empty-capable plugin components. Retain construction from `empty_string` and
`identifier`, as well as the existing valid empty default state. These are lasting public value
constructors, not temporary compatibility overloads on each consuming API.

Every textual path maps empty text to the `empty_string` alternative and non-empty text to an
`identifier`. Named text therefore acquires the same ASCII validation as any other identifier
when strict validation is activated. Invalid non-empty text must never fall back to the empty
alternative. Length-bearing string inputs must be validated in full, including embedded NULs;
the `char const*` overload retains the existing null-terminated C-string boundary. Configuration
and Python conversions must retain lengths rather than route sized input through that overload.

This preserves the existing literal syntax for explicit providers and selectors while making
runtime string conversion explicit:

```cpp
registration.output_product("input", "hits", "job");
registration.output_product("input", "", "job");
registration.output_product("input", suffix_t{suffix_text}, "job");
registration.output_product("input", "hits"_id, "job");
registration.output_product("input", empty_string{}, "job");

product_selector{.suffix = "hits"}; // A named criterion.
product_selector{.suffix = ""};     // An explicit empty criterion.
product_selector{};                 // No suffix criterion.
```

Required-name APIs continue to use `identifier` and reject empty text. Accepting `""` for an
empty-capable plugin component does not permit an empty actual plugin name; the owning API still
determines whether absence is allowed. The constructors convert text to values, not selection
policy: only an omitted optional selector field represents an unrestricted criterion.

Alternative considered: require domain alternatives at every typed call site. Rejected because
existing provider and selector calls intentionally accept literals, and forcing their migration
would introduce an additional source break without improving validation. Explicit-only textual
construction would also break those calls. Implicit conversion from runtime string objects was
rejected to preserve the existing `identifier` distinction between C strings and string objects.

### Apply domain types to product metadata and creators

`experimental::product_specification` will store and expose its suffix as `suffix_t`; construction
from text and default-output-suffix generation will construct `empty_string` for an empty suffix
and an `identifier` otherwise. Product-specification parsing and string formatting will preserve
this representation without manufacturing an empty `identifier`.

`experimental::algorithm_name` will store its plugin component as `identifier_or_empty` and its
algorithm component as `identifier`. Parsing an unqualified algorithm name produces an empty
plugin component and a non-empty algorithm; a qualified name requires non-empty valid plugin and
algorithm components. Supplying an empty actual plugin name is invalid, not an instruction to
omit the plugin: an empty component represents absence only where the owning API permits it.
For example, `make_algorithm_name` constructs a validated plugin identifier from a configured
`module_label`, but uses `empty_string` only for unconfigured registration with a null configuration
pointer (`../srcs/phlex/phlex/core/detail/make_algorithm_name.cpp`). A non-null configuration that
omits `module_label` remains an error. Accept `find_hits` and `reco:find_hits`; reject `:find_hits`,
`reco:`, and empty creator names.

Store creator absence in `product_specification`, using an owning optional creator rather than
an empty `algorithm_name`. Suffix-only text (`hits`, `/hits`, `""`, and `/`) retains this state.
It remains valid for product storage and exact lookup, not just parsing. Equality, ordering, and
hashing distinguish absence from every present creator; absence is not a wildcard. Creator and
component accessors must handle this state explicitly. Handles retain empty borrowed text views
for absent creator components. Provider construction reports an absent creator as an error.

Producer catalogs and registration APIs that store a product suffix or plugin component will use
`suffix_t` or `identifier_or_empty` respectively; required plugin names use `identifier`.
`consumer::plugin()` must return the new component type. The composed graph node names used as
keys in `index_router` remain composed names, not generic identifiers: `plugin:algorithm` contains
a colon and cannot be converted to an `identifier`. Use a valid internal source creator instead
of `[Source]` in `product_store`. Handles already borrow their product specification
and return `std::string_view` for suffix and creator components
(`../srcs/phlex/phlex/model/handle.hpp`); retain those textual-view accessors, converting from
the borrowed specification's validated values without introducing another stored domain value.
Names for concrete layers, stages, algorithms, and other required identifiers remain `identifier`.

Alternative considered: change only product specifications and convert at every storage boundary.
Rejected because registration, catalog lookup, and creator formatting would otherwise continue
encoding empty values as invalid identifiers. Changing the handle's public views to domain types
was also rejected because it needlessly breaks callers of its borrowed textual metadata.

### Migrate the current producer and typed-node architecture

Use the independent `producer` interface, whose contract exposes `name()`, `output_port()`, and
`output()`. `producer_catalog(std::span<producer* const>)` constructs suffix-keyed entries from
those output specifications; `node_catalog::producers()` collects transforms, translators, folds,
and unfolds. Migrate this constructor, suffix keys, lookup, pointer trackers, and diagnostics
together (`../srcs/phlex/phlex/core/producer.hpp`, `producer_catalog.*`, and `node_catalog.cpp`).
The older `producing_nodes()` helper no longer exists and must not be recreated for this change.
The direct regression in `../srcs/phlex/test/producer_catalog_test.cpp` must continue demonstrating
that catalog construction works with an independent producer implementation, including the new
suffix values.

Migration must inspect both retained runtime-base policies in `declared_*.cpp` and their extracted
typed implementations: `transform_node.hpp`, `fold_node.hpp`, `unfold_node.hpp`,
`observer_node.hpp`, `predicate_node.hpp`, and `translator_node.hpp` under
`../srcs/phlex/phlex/core/`. Typed nodes construct joins and, where applicable, output
specifications, so checking only the declared bases is insufficient. Keep today's layer policy:
single-input omission remains allowed where supported; multiple inputs, folds, and unfolds retain
their required-layer errors. Inspect `multilayer_join_node.hpp` and `fold_join_node.hpp` for impact
without introducing an unrelated optional-layer redesign.

Alternative considered: restore the old catalog helper or move typed logic back into declared
bases to simplify migration. Rejected because the domain transition fits the merged architecture
and does not justify reversing its separation of interfaces from typed implementations.

### Preserve current translator metadata during the domain migration

At the audited baseline, `translate()` creates a graph producer. Its output specification uses the
translator's creator and registered output suffix, while its output store keeps the input
data-cell index and records the translator's execution stage. Preserve this behavior while
changing creator components and suffix values (`../srcs/phlex/phlex/core/glue.hpp`,
`declared_translator.hpp`, and `translator_node.hpp`). Do not replace those metadata with the
input creator, suffix, or stage as part of identifier migration.

Keep the creator, suffix, layer, and stage assertions in the end-to-end translator regression in
`../srcs/phlex/test/function_registration_test.cpp`. Rename its invalid actual stage
`translator-test` to a valid fixture such as `translator_test` before strict validation, updating
both registration and the expected stage. The test must continue verifying that an observer can
consume the translator's catalogued output.

The separate `../translator-type-conversion/proposal.md` proposes preserving input metadata,
recording translator identity, and making `translate()` registration-only. Those semantic changes
remain outside this change. If that work lands before this migration is finished, reconcile the
overlapping implementation and regression tests against its accepted behavior without silently
importing its redesign into the identifier contract.

Alternative considered: combine both migrations because they touch translator code. Rejected
because naming-value changes neither require nor establish the separate conversion semantics.
Keeping the changes separate makes metadata regressions attributable to the correct work.

### Convert output suffixes at the appropriate registration boundary

Keep fluent `output_product_suffixes(...)` accepting strings, including the existing empty default;
convert to `suffix_t` when those strings become product specifications. Change the explicit
provider's `output_product(...)` suffix parameter from `identifier` to `suffix_t`, so callers can
explicitly supply an empty value. Both paths produce an explicit empty suffix when appropriate,
not an omitted selection criterion (`../srcs/phlex/phlex/core/upstream_predicates.hpp`,
`../srcs/phlex/phlex/core/registration_api.hpp`).

The provider remains a typed API even though the hybrid value constructors preserve raw C-string
arguments, including `""`. Existing `identifier` arguments also remain usable through construction
of the named alternative. Runtime suffix strings use explicit `suffix_t` construction; the fluent
API continues accepting strings directly. Test these call forms together so signature migration
does not inadvertently require rewriting valid literal providers or selector initializers.

Alternative considered: change the fluent API to accept only `suffix_t`. Rejected to preserve its
convenient textual boundary and avoid breaking existing string-based registration calls.

### Keep FORM's persisted text and validate it on conversion

The FORM writer converts `suffix_t` to its textual representation instead of calling the
identifier-only `trans_get_string()` accessor. The source converts its configured requested
suffixes to `empty_string` for `""` and a validated `identifier` otherwise
(`../srcs/phlex/form/form_module.cpp`, `../srcs/phlex/form/form_source.cpp`). Validate non-empty
persisted suffixes when the reader encounters them, before constructing product metadata.
This is distinct from validating the configured `products` list. Keep the persisted text; report
invalid legacy suffixes with file/product context instead of rewriting or accepting them.

Determine the recovered product's layer from existing `indices(actual_creator, stage, product)`
information. Parse the canonical index strings into data-cell indices and require one consistent
terminal data-layer name across the relevant stream. The Job index `[]` establishes the Job layer.
Missing indices, conflicting terminal layers, or information that cannot establish one layer
cause an error, even when the selector supplies a layer. Use the established name for matching
and provider registration. Keep the stored creator for index lookup and the configured advertised
creator for selection and registration. Validate each supplied plugin or algorithm override,
including empty values, even when the existing fallback would ignore that override.

The current index interface exposes creator-and-stage streams, not product-specific dictionary
lookup. Thus some files with valid product-specific metadata will fail this narrower recovery
rule. This limitation is accepted. Do not decode `hierarchy_key` by splitting on underscores:
valid layer names can contain underscores, and flattened hierarchy keys can collide.

Alternative considered: accept invalid old suffixes in a special read-only representation.
Rejected because it would bypass the new name invariant and complicate matching across sources.
Product-specific dictionary lookup was also considered. It is outside this change; the existing
index information is used with explicit errors when it cannot establish a layer.

### Preserve selector tri-state semantics at conversion boundaries

`product_selector::suffix` will become `std::optional<suffix_t>`. `std::nullopt` remains an
unspecified selector suffix that matches any product suffix; `empty_string` is an explicit suffix
that matches only an empty product suffix; and an `identifier` matches its corresponding named
suffix. Matching, ordering, equality, and diagnostic rendering must preserve all three states.

Keep the creator criterion as an optional single `identifier`. It matches either the plugin name
or the algorithm name, not a qualified `plugin:algorithm` pair. Omission removes the creator
restriction; supplied qualified text is invalid. Diagnostic placeholders such as `[ANY]` and `*`
are display text, not identifiers.

JSON configuration and the Python selector parser will first test whether the `suffix` field is
present. If it is absent, they produce `std::nullopt`; if present with `""`, they produce
`empty_string`; otherwise they construct a validated `identifier`. Required textual fields use
`identifier` construction and therefore reject empty or invalid ASCII input, including a supplied
actual plugin name. Absence of an optional plugin component produces `empty_string`, not an empty
`identifier`; it does not make actual plugin names empty-capable.

Python provider output registration needs a separate concrete-metadata conversion, rather than
reusing the input-selector parser. Construct its creator directly as `algorithm_name`, so
`reco:find_hits` remains valid there without expanding selector matching. Extract Python text
with its length and translate validation exceptions into Python errors. An embedded NUL is
invalid input, not a terminator that permits silent truncation.

An empty Python output suffix currently generates `_py`, which violates the letter-first rule.
Choose a valid private intermediate suffix for this case. Change the callback output registration
and its output-translator selector together so they use the same name. Keep the final user-visible
suffix empty. Do not change the existing intermediate names for valid named outputs unnecessarily.

Alternative considered: normalize an explicit empty suffix to omitted at parsing time. Rejected
because it changes selector matching and loses the distinction required by the proposal.

## Risks / Trade-offs

- [Existing default constructors and aggregate initialization rely on an empty `identifier`] →
  Locate all default and empty constructions during migration; replace them with an explicit
  empty-domain value, optional state, or valid initialization according to the owning field.
- [Existing `"*"_id` sentinels are not valid identifiers] → Represent omitted layer/suffix query
  state separately in `products_consumer`, `index_router`, and `producer_catalog`; test that
  wildcard matching is unchanged.
- [FORM source currently derives a concrete layer from a possibly omitted selection] → Obtain the
  produced product's actual layer for matching and provider creation or report missing metadata
  clearly, without converting absence into a name.
- [Public C++ and Python plugin APIs change source types] → Update all exposed signatures and
  boundary conversions together, document the breaking conversion, and cover representative
  plugin calls in tests.
- [Hybrid constructors accidentally change call-site or empty-value semantics] → Test implicit
  C-string conversion, explicit string-object construction, domain-value construction, and selector
  omission separately. Invalid non-empty text remains an error, not an empty alternative.
- [Node and catalog migrations target the pre-refactor implementation] → Use the audited producer
  interface and extracted typed headers; run direct producer-catalog and translator-registration
  regressions at the named checkpoints.
- [Overlapping translator work changes metadata during the naming migration] → Preserve the audited
  behavior in this change and reconcile any later merge with the separate accepted conversion plan.
- [Hashing or ordering an empty-or-identifier value becomes inconsistent with equality] → Define
  and test equality, ordering, and hash behavior as a single value contract before migrating
  associative containers.
- [Parser changes accidentally collapse explicit empty and omitted suffixes] → Add configuration
  and Python tests that assert distinct resulting selectors and matching behavior.
- [An empty-capable plugin component is mistaken for permission to register an empty plugin name] →
  Keep actual plugin names as `identifier` and test that supplied empty names are rejected while
  unqualified creators and registration without a plugin retain an explicit empty component.
- [Product specifications without creators are used as exact storage keys] → Preserve this valid
  state at `product_specification`; keep exact lookup and empty handle views, and reject absent
  creators at provider registration.
- [Previously persisted FORM suffixes may violate the new ASCII syntax] → Reject invalid non-empty
  values with contextual errors; preserve valid and empty suffixes without changing the file format.

## Migration Plan

1. Introduce and test the explicit empty value, empty-or-identifier value, and suffix alias. Include
   text conversion, comparison, ordering, and hashing. These values already exist at the audited
   baseline; add and test the hybrid textual constructors before migrating their consuming APIs.
   Leave strict identifier validation and default-constructor removal until existing empty and
   wildcard uses have migrated.
2. Migrate creator components and owning creator absence. Then migrate product suffix storage,
   fluent conversion, selector criteria, the current producer-catalog constructor, keys and trackers,
   typed-node output construction, and FORM writer text conversion. Retain handle metadata as
   borrowed text views and preserve the current translator metadata behavior.
3. Retire nameless layers, consumer/router wildcard identifiers, and composed graph-name keys.
   Migrate explicit-provider, JSON, Python, and FORM source boundaries with their focused tests
   and public documentation. Preserve supported literal calls through the hybrid constructors.
   Assign known invalid fixtures and internal names, including the translator regression's stage,
   to their owning tasks before strict validation.
4. Activate ASCII validation for every construction path, including `_id` and `_idq`, and remove
   public default construction. Audit remaining constructions in `phlex/`, `plugins/`, `form/`,
   and `test/` for invalid names and domain misuse.
5. Verify each coordinated transition at its named checkpoint using the build-and-test policy
   below; a successful test-only rerun does not establish that the build is current.

Checkpoint A deliberately builds only the migrated framework and selected test targets after the
core and JSON transitions, because Python and FORM source migration follows it. Discover the
configured build directory with `spack mpd info --build phlex-work-dir`; keep project selection,
the targeted `cmake --build`, CTest listing, and execution in one fail-closed shell invocation.
List the intended CTests with `-N`, confirm a non-empty selection, and execute the same selection
with `--no-tests=error`. Include `producer_catalog` and `function_registration` alongside the
existing A selection, so both independent producers and stage-aware translators are exercised.
Verify empty/named hybrid construction and supported literal call forms at A; defer only the
identified strict-name rejection cases to C. This is an explicit scoped-build exception to the
default verifier, which always performs a full build and has no target-selection option.

At Checkpoint B, after Python and FORM source migration, use
`bash srcs/phlex/scripts/verify.sh --project phlex-work-dir --tests '<checkpoint-B-pattern>'`
from the workspace root. The pattern must include A's tests and the focused Python and FORM tests
with their fixture dependencies. At Checkpoint C, after strict validation, run
`bash srcs/phlex/scripts/verify.sh --project phlex-work-dir` for a full build and suite, including
all deferred rejection cases. Follow `../srcs/phlex/scripts/README.md` and use `--verbose` for
detailed CTest evidence. Both checkpoints require success before continuing; record unavailable
optional test coverage explicitly. Do not replace required validation with an empty selection or
truncated build/test output.

Use small dependent tasks with named build checkpoints in `tasks.md`. Each task delivers its
focused tests and documentation; record runtime checks that must wait for a checkpoint. A task
does not claim that an unrun test passed. Shared signature transitions may temporarily prevent
compilation within their declared group. The creator/accessor transition, suffix/selector/catalog
transition, and paired Python intermediate naming changes need coordinated changes because their
callers must agree on the types or names. Do not add temporary compatibility APIs just to compile
between these dependent tasks. Existing `identifier` tests link configuration, so they are not an
isolated model-only build.

The change is source-breaking for invalid identifier constructions, owners relying on invalid
default names, and callers coupled to the changed suffix, plugin-component, or creator-accessor
signatures. It does not require a blanket rewrite of valid literal suffix calls or existing valid
`identifier` arguments: the hybrid constructors and named-alternative constructors preserve those
forms. Runtime string objects require explicit domain construction at typed boundaries. Actual
plugin names remain non-empty identifiers, and the fluent string API and handle textual views
remain unchanged. Rollback consists of reverting the cohesive API and boundary-conversion change.
The FORM file format remains textual and requires no rewrite, but old files with invalid non-empty
names will no longer be readable under the new validation rule.
