## Context

`experimental::identifier` currently stores arbitrary strings and is default-constructible, so
the model uses an empty identifier for three different states: a meaningful empty product suffix,
an absent plugin component, and an invalid required name. `algorithm_name`,
`experimental::product_specification`, `product_selector`, configuration conversion, and the Python
plugin bridge consequently each carry or reconstruct those meanings independently.

This design refines the naming model in `proposal.md`, using the product-specification and
product-selection vocabulary in `CONTEXT.md` and the labeling roles in
`design_wiki/seeds/how-data-products-are-labeled.md`. A complete label for a created product has
a non-empty layer name; a selection may omit the layer to match any layer. The current
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
- Preserve three selector states: omitted suffix (`std::nullopt`), explicit empty suffix, and
  explicit non-empty suffix.
- Keep a selection's omitted layer distinct from a concrete, non-empty layer name.
- Keep C++, JSON configuration, and Python conversion at clear boundaries that construct the
  correct domain type from textual input.

**Non-Goals:**

- Add distinct runtime validation or behavior to `suffix_t` and `plugin_name_t`; they remain
  aliases with the same value semantics.
- Change the existing creator matching or product-selection rules beyond making their empty and
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
with a data-cell or provider layer to form the complete label. A product selection's layer
criterion remains optional: omitted matches any concrete layer, while a supplied name is a valid
`identifier`; an empty string is neither a valid layer name nor a wildcard. This distinction
matches `CONTEXT.md` and the matching API in `../srcs/phlex/phlex/core/product_selector.hpp`.

`data_cell_index::make_child("")` and its nameless-cell tests must cease to succeed. The existing
`"*"_id` sentinels for omitted layers or suffixes in the consumer, router, and producer catalog
must become distinct wildcard/query state or display text rather than invalid `identifier` values;
preserve the existing any-layer and any-suffix selection behavior.

Alternative considered: encode an omitted selection layer with an empty identifier or `"*"`.
Rejected because neither is a valid name, and a wildcard is not a concrete layer.

### Model meaningful emptiness as a sum value

Introduce `empty_string` as the sole explicit marker for an empty textual value, and introduce
`identifier_or_empty` as the value type containing either `empty_string` or a valid `identifier`.
It will expose the common value operations required by existing suffix and plugin consumers:
comparison, ordering, conversion/formatting to text, and hashing. Hash and comparison semantics
must distinguish the empty alternative from every non-empty identifier while retaining stable
value behavior for maps and unordered containers.

Define `suffix_t` and `plugin_name_t` as aliases of `identifier_or_empty`. Use the aliases in
public model and plugin-facing signatures to communicate the domain even though they currently
share implementation and validation.

Alternative considered: use `std::optional<identifier>` for meaningful emptiness. Rejected because
an optional already means an omitted selector field; using it for both would collapse explicit
empty and unspecified suffixes. A raw `std::string` was also rejected because it would weaken the
validity guarantee of the non-empty alternative.

### Apply domain types to product metadata and creators

`experimental::product_specification` will store and expose its suffix as `suffix_t`; construction
from text and default-output-suffix generation will construct `empty_string` for an empty suffix
and an `identifier` otherwise. Product-specification parsing and string formatting will preserve
this representation without manufacturing an empty `identifier`.

`experimental::algorithm_name` will store its plugin component as `plugin_name_t` and its
algorithm component as `identifier`. Parsing an unqualified algorithm name produces an empty
plugin component and a non-empty algorithm; a qualified name requires non-empty valid plugin and
algorithm components. The no-creator/default representation used by product-specification parsing
must remain explicit at its owning boundary rather than depending on an empty `algorithm_name`.

Producer catalogs, registration APIs, and graph metadata that store a product suffix or plugin
component will use their respective aliases. Handles already borrow their product specification
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
identifier-only `trans_get_string()` accessor; the reader constructs `empty_string` for `""` and
a validated `identifier` for non-empty suffixes (`../srcs/phlex/form/form_module.cpp`,
`../srcs/phlex/form/form_source.cpp`). Preserve the persisted textual format, but reject a legacy
non-empty suffix that violates the new ASCII rule with a clear error; do not grandfather or rewrite
it. The FORM source must not materialize an omitted selection layer as an invalid identifier:
match the selection against the actual layer supplied by product/cell metadata when available,
and make any missing concrete layer explicit rather than silently inventing one.

Alternative considered: accept invalid old suffixes in a special read-only representation.
Rejected because it would bypass the new name invariant and complicate matching across sources.

### Preserve selector tri-state semantics at conversion boundaries

`product_selector::suffix` will become `std::optional<suffix_t>`. `std::nullopt` remains an
unspecified suffix that matches any suffix; `empty_string` is an explicit suffix that matches only
an empty product suffix; and an `identifier` matches its corresponding named suffix. Matching,
ordering, equality, and diagnostic rendering must preserve all three states.

JSON configuration and the Python selector parser will first test whether the `suffix` field is
present. If it is absent, they produce `std::nullopt`; if present with `""`, they produce
`empty_string`; otherwise they construct a validated `identifier`. Required textual fields use
`identifier` construction and therefore reject invalid ASCII input. Python-generated converter
suffixes follow the same conversion rule when prefixes or user suffixes produce an empty value.

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
- [Legacy textual product specifications encode an omitted creator] → Keep parsing compatibility
  at `product_specification` while representing the absence at that type's boundary, rather than
  constructing an invalid identifier.
- [Previously persisted FORM suffixes may violate the new ASCII syntax] → Reject invalid non-empty
  values with contextual errors; preserve valid and empty suffixes without changing the file format.

## Migration Plan

1. Introduce and unit-test the model value types, including ASCII name validation, missing or
   invalid first characters, empty names, text conversion, comparison, ordering, and hashing.
2. Migrate `algorithm_name`, `experimental::product_specification`, registration, catalogs, and
   selector storage to the appropriate domain types; keep handle metadata as borrowed text views.
   Remove nameless-cell creation and replace invalid default and `"*"_id` sentinel values without
   changing omitted-selection semantics.
3. Convert fluent string and explicit-provider output suffixes at their respective boundaries.
   Update JSON configuration, Python bindings, and FORM input/output conversions; test valid and
   invalid persisted suffixes and optional selection layers alongside empty/omitted suffixes.
4. Build with `spack mpd select --project phlex-work-dir && spack mpd build` and test with
   `spack mpd select --project phlex-work-dir && spack mpd test -j 12`.

The change is source-breaking for callers that construct invalid `identifier` values or pass one
where a suffix or plugin name is now expected; the fluent string API and handle textual views
remain. Rollback consists of reverting the cohesive API and boundary-conversion change. The FORM
file format remains textual and requires no rewrite, but old files with invalid non-empty names
will no longer be readable under the new validation rule.
