## Why

`identifier` currently accepts arbitrary strings, including the empty string, even though its
instances represent names in several distinct domains with different validity rules. An actual
name must be non-empty and follow the agreed ASCII syntax. By contrast, an empty product suffix
is meaningful, and a product selector with no suffix criterion matches any suffix rather than
only an empty one. The creator's plugin component may also be empty. The framework needs
domain-aware names that preserve these distinctions while rejecting invalid identifiers.

## What Changes

- Constrain `experimental::identifier` to non-empty ASCII text matching
  `^[A-Za-z][A-Za-z0-9_]*$`; C++ keywords are not excluded.
- Identifiers can be compared for equality and can be ordered.
- Identifiers are hashable.
- Introduce `empty_string` as the explicit representation of a meaningful empty textual value.
- Introduce an `identifier_or_empty` value type that can hold either `empty_string` or a valid
  `identifier`.
- Define `suffix_t` as an alias of `identifier_or_empty` while its validity
  and behavior remains the same.
- Replace identifier uses that model a product suffix or the optional plugin component with the
  appropriate domain type; retain `identifier` where a non-empty generic identifier remains the
  correct model. Plugin names can not be empty, but some components may have optional plugin
  names.
- Preserve the distinction between an unspecified selector field and an explicit empty suffix.
- Distinguish complete product specifications, whose layer names must be present and non-empty,
  from product selectors, whose omitted layer criterion matches any layer. A specified selector
  layer must also be non-empty.
- Keep `CURRENT` valid as a generic identifier but reserved as the name of an actual execution
  stage; retain its placeholder uses.
- Keep handle metadata accessors as textual views of the underlying specification. Keep the
  fluent output-suffix registration API textual, convert at specification creation, and accept
  `suffix_t` directly in the explicit-provider API.
- Update C++ APIs, configuration parsing, Python bindings, FORM text conversions, and tests for
  the revised naming model without changing the persisted FORM format.

**BREAKING**: Public model and explicit-provider APIs that accept or expose suffixes or plugin
names will use the new domain types; the handle's textual-view API and the fluent string
registration API remain unchanged. Code that constructed an empty or otherwise invalid
`identifier` will need to use a valid name or the explicit empty-value representation where that
is semantically valid.

## Capabilities

### New Capabilities

- `identifier-domain-types`: Validated identifier values and explicit domain types for empty or
  named product suffixes and plugin-name components.

### Modified Capabilities

- None.

## Impact

- **Shared design (`phlex-design`)**: Capture the vocabulary of `CONTEXT.md` and align with
  `design_wiki/seeds/how-data-products-are-labeled.md`, following `AGENTS.md`.
- **Framework (`../srcs/phlex`, following `../srcs/phlex/AGENTS.md` and
  `../srcs/phlex/DEVELOPING.md`)**: Model (`phlex/model/`) identifiers, algorithm names, product
  specifications, and data-layer naming paths adopt the revised value model; handles retain their
  textual-view accessors.
- **Core (`phlex/core/`)**: product selectors, registration APIs, routing, and graph-facing
  product metadata distinguish required identifiers from suffix and plugin-name values. Fluent
  string output suffixes and typed explicit-provider suffixes converge when specifications form.
- **Configuration and Python (`phlex/configuration.*`, `plugins/python/`)**: textual inputs will
  be converted at their boundaries while retaining the intended empty-value semantics.
- **FORM (`form/`)**: Convert textual suffixes on input and output without changing the stored
  textual representation.
- **Tests (`test/`)**: identifier validation, empty-versus-unspecified selection, naming
  comparisons, formatting, hashing, the non-empty specified layer, FORM round trips, and affected
  public API behavior require coverage. No changes to `../srcs/phlex-examples` are planned.

## Non-goals

- Changing the hierarchy of data layers or the existing stage-specific `CURRENT` reservation
  beyond preserving it alongside generic identifier validation. Nameless created layers are
  intentionally ruled out by the new identifier contract.
- Changing product-selector matching rules other than preserving the existing distinction
  between an unspecified suffix and an explicitly empty suffix.
- Implementing unrelated translator-type-conversion work.
