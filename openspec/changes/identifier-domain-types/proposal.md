## Why

`identifier` currently accepts arbitrary strings, including the empty string, even though its
instances represent names in several distinct domains (e.g. algorithm names, suffix names),
with different validity rules. That lets
`identifier` have states that should be illegal for some of its uses; for example, the name
by which a user algorithm is registered must not be an empty string. In other cases, an
empty string representation is allowed; a suffix is allowed to be represented by an empty
string. And an empty suffix represents a different thing from the failure to have a suffix,
because many data products will not be associated with a suffix.
The framework needs domain-aware names that preserve the
meaning of an empty suffix or absent plugin component while rejecting invalid identifiers.

## What Changes

- Constrain `experimental::identifier` to represent a valid, non-empty identifier.
  An identifier is always printable.
- Identifiers can be compared for equality and can be ordered.
- Identifiers are hashable.
- Introduce `empty_string` as the explicit representation of a meaningful empty textual value.
- Introduce an `identifier_or_empty` value type that can hold either `empty_string` or a valid
  `identifier`.
- Define `suffix_t` and `plugin_name_t` as aliases of `identifier_or_empty` while their validity
  and behavior remain the same.
- Replace identifier uses that model a product suffix or the optional plugin component with the
  appropriate domain type; retain `identifier` where a non-empty generic identifier remains the
  correct model.
- Preserve the distinction between an unspecified selector field and an explicit empty suffix.
- Update C++ APIs, configuration parsing, Python bindings, and tests for the revised naming
  model.

**BREAKING**: Public model and plugin-facing APIs that accept or expose suffixes or plugin names
will use the new domain types. Code that constructed an empty `identifier` will need to use the
explicit empty-value representation where that is semantically valid.

## Capabilities

### New Capabilities

- `identifier-domain-types`: Validated identifier values and explicit domain types for empty or
  named product suffixes and plugin-name components.

### Modified Capabilities

- None.

## Impact

- **Model (`phlex/model/`)**: `identifier`, `algorithm_name`, product specifications, handles,
  and data-layer naming paths will adopt the revised value model where appropriate.
- **Core (`phlex/core/`)**: product selectors, registration APIs, routing, and graph-facing
  product metadata will distinguish generic identifiers from suffix and plugin-name values.
- **Configuration and Python (`phlex/configuration.*`, `plugins/python/`)**: textual inputs will
  be converted at their boundaries while retaining the intended empty-value semantics.
- **Tests (`test/`)**: identifier validation, empty-versus-unspecified selection, naming
  comparisons, formatting, hashing, and affected public API behavior require coverage.

## Non-goals

- Changing the meaning or hierarchy of data-layer, stage, or algorithm names beyond the common
  identifier validation rule.
- Introducing distinct runtime behavior for `suffix_t` and `plugin_name_t` while they share the
  same value semantics.
- Changing product-selection matching rules other than preserving the existing distinction
  between an unspecified suffix and an explicitly empty suffix.
- Implementing unrelated translator-type-conversion work.
