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

### Convert output suffixes at the appropriate registration boundary

Keep fluent `output_product_suffixes(...)` accepting strings, including the existing empty default;
convert to `suffix_t` when those strings become product specifications. Change the explicit
provider's `output_product(...)` suffix parameter from `identifier` to `suffix_t`, so callers can
explicitly supply an empty value. Both paths produce an explicit empty suffix when appropriate,
not an omitted selection criterion (`../srcs/phlex/phlex/core/upstream_predicates.hpp`,
`../srcs/phlex/phlex/core/registration_api.hpp`).

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
   text conversion, comparison, ordering, and hashing. Leave strict identifier validation and
   default-constructor removal until existing empty and wildcard uses have migrated.
2. Migrate creator components and owning creator absence. Then migrate product suffix storage,
   fluent conversion, selector criteria, catalog keys and trackers, and FORM writer text conversion.
   Retain handle metadata as borrowed text views.
3. Retire nameless layers, consumer/router wildcard identifiers, and composed graph-name keys.
   Migrate explicit-provider, JSON, Python, and FORM source boundaries with their focused tests
   and public documentation. Assign known invalid fixtures and internal names to their owning tasks.
4. Activate ASCII validation for every construction path, including `_id` and `_idq`, and remove
   public default construction. Audit remaining constructions in `phlex/`, `plugins/`, `form/`,
   and `test/` for invalid names and domain misuse.
5. Build with `spack mpd select --project phlex-work-dir && spack mpd build` and test with
    `spack mpd select --project phlex-work-dir && spack mpd test -j 12`.

Use small dependent tasks with named build checkpoints in `tasks.md`. Each task delivers its
focused tests and documentation; record runtime checks that must wait for a checkpoint. A task
does not claim that an unrun test passed. Shared signature transitions may temporarily prevent
compilation within their declared group. The creator/accessor transition, suffix/selector/catalog
transition, and paired Python intermediate naming changes need coordinated changes because their
callers must agree on the types or names. Do not add temporary compatibility APIs just to compile
between these dependent tasks. Existing `identifier` tests link configuration, so they are not an
isolated model-only build.

The change is source-breaking for callers that construct invalid `identifier` values or pass one
where `suffix_t` or an empty-capable `identifier_or_empty` plugin component is now expected; actual
plugin names remain non-empty identifiers, and the fluent string API and handle textual views
remain. Rollback consists of reverting the cohesive API and boundary-conversion change. The FORM
file format remains textual and requires no rewrite, but old files with invalid non-empty names
will no longer be readable under the new validation rule.
