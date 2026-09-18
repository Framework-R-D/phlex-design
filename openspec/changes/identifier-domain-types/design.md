## Context

`experimental::identifier` currently stores arbitrary strings and is default-constructible, so
the model uses an empty identifier for three different states: a meaningful empty product suffix,
an absent plugin component, and an invalid required name. `algorithm_name`,
`detail::product_specification`, `product_selector`, configuration conversion, and the Python
plugin bridge consequently each carry or reconstruct those meanings independently.

This design refines the naming model described in `proposal.md`. It follows the data-product
metadata roles in `design_wiki/seeds/how-data-products-are-labeled.md`: the creator's plugin
component and a product suffix can be empty values, while a supplied algorithm name, layer name,
stage name, or generic identifier cannot.

## Goals / Non-Goals

**Goals:**

- Make construction of a generic `identifier` reject empty textual input, so required names are
  valid by construction.
- Represent a semantically meaningful empty string explicitly and make empty-or-named values
  usable anywhere suffixes and plugin components are stored, compared, formatted, or hashed.
- Preserve three selector states: omitted suffix (`std::nullopt`), explicit empty suffix, and
  explicit non-empty suffix.
- Keep C++, JSON configuration, and Python conversion at clear boundaries that construct the
  correct domain type from textual input.

**Non-Goals:**

- Add distinct runtime validation or behavior to `suffix_t` and `plugin_name_t`; they remain
  aliases with the same value semantics.
- Change the existing creator matching or product-selection rules beyond making their empty and
  omitted inputs explicit.
- Redesign data-layer, stage, or algorithm naming beyond enforcing non-empty generic identifiers.
- Introduce the separate translator-type-conversion work.

## Decisions

### Make `identifier` a required, non-empty value

`experimental::identifier` will retain its string storage, precomputed hash, equality, ordering,
formatting, hashing, and `_id` literal interface. Its public construction paths will reject an
empty string and it will no longer provide a public invalid/empty default state. Any internal
default construction required by an owning type must be removed or replaced by a domain-valid
initialization path.

This places the invariant at the value boundary instead of relying on scattered `empty()` checks
such as those currently in `creator_name` and `layer_name`. It also prevents values parsed from
JSON and Python from bypassing the same rule.

Alternative considered: retain empty `identifier` and add validation at every API that requires a
name. Rejected because APIs already disagree about emptiness and future call sites could silently
reintroduce invalid states.

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

`detail::product_specification` will store and expose its suffix as `suffix_t`; construction from
text and default-output-suffix generation will construct `empty_string` for an empty suffix and
an `identifier` otherwise. Product-specification parsing and string formatting will preserve this
representation without manufacturing an empty `identifier`.

`experimental::algorithm_name` will store its plugin component as `plugin_name_t` and its
algorithm component as `identifier`. Parsing an unqualified algorithm name produces an empty
plugin component and a non-empty algorithm; a qualified name requires non-empty valid plugin and
algorithm components. The no-creator/default representation used by product-specification parsing
must remain explicit at its owning boundary rather than depending on an empty `algorithm_name`.

Handles, producer catalogs, registration APIs, and graph metadata that carry a product suffix or
plugin component will use their respective aliases. Names for layers, stages, algorithms, and
other required identifiers remain `identifier`.

Alternative considered: change only product specifications and convert at the edges. Rejected
because handles, registration, catalog lookup, and creator formatting would otherwise continue to
encode empty values as invalid identifiers.

### Preserve selector tri-state semantics at conversion boundaries

`product_selector::suffix` will become `std::optional<suffix_t>`. `std::nullopt` remains an
unspecified suffix that matches any suffix; `empty_string` is an explicit suffix that matches only
an empty product suffix; and an `identifier` matches its corresponding named suffix. Matching,
ordering, equality, and diagnostic rendering must preserve all three states.

JSON configuration and the Python selector parser will first test whether the `suffix` field is
present. If it is absent, they produce `std::nullopt`; if present with `""`, they produce
`empty_string`; otherwise they construct a validated `identifier`. Required textual fields use
`identifier` construction and therefore reject empty input. Python-generated converter suffixes
follow the same conversion rule when prefixes or user suffixes produce an empty value.

Alternative considered: normalize an explicit empty suffix to omitted at parsing time. Rejected
because it changes selector matching and loses the distinction required by the proposal.

## Risks / Trade-offs

- [Existing default constructors and aggregate initialization rely on an empty `identifier`] →
  Locate all default and empty constructions during migration; replace them with an explicit
  empty-domain value, optional state, or valid initialization according to the owning field.
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

## Migration Plan

1. Introduce and unit-test the new model value types, including invalid empty-identifier input,
   text conversion, comparison, ordering, formatting, and hashing.
2. Migrate `algorithm_name`, `product_specification`, handles, registration, catalogs, and
   selector storage to their domain aliases; replace empty identifier construction at each call
   site with `empty_string` or an optional state.
3. Update JSON configuration and Python binding conversion so missing and explicit-empty suffixes
   remain distinct, then update public API and integration tests.
4. Build with `spack mpd select --project phlex-work-dir && spack mpd build` and test with
   `spack mpd select --project phlex-work-dir && spack mpd test -j 12`.

The change is source-breaking for callers that construct empty `identifier` values or pass them
where a suffix or plugin name is now expected. Rollback consists of reverting the cohesive API and
boundary-conversion change; no persisted data migration is required because configuration remains
textual and its prior syntax is preserved.
