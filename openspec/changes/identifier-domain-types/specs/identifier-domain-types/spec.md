# Spec Delta

## Purpose

Define validated names and explicit empty-value semantics for Phlex product labels and
product selectors, so naming remains consistent across registration, configuration, plugins, and
storage.

## ADDED Requirements

### Requirement: Valid generic identifiers

The framework SHALL accept an `identifier` only when its text matches
`^[A-Za-z][A-Za-z0-9_]*$`, regardless of whether that text is a C++ keyword. It SHALL reject
empty strings, non-ASCII characters, and all other text. Every successfully created identifier
SHALL retain its text, support formatting, equality and ordering, and be hashable with equal
values producing equal hashes. A public API SHALL NOT expose an invalid default identifier.

#### Scenario: Accept ASCII names and keywords

- **WHEN** a caller provides `A`, `run_2`, or `class` as an identifier
- **THEN** the framework accepts each name and its formatted text is unchanged

#### Scenario: Reject invalid identifiers

- **WHEN** a caller provides `""`, `2run`, `_run`, `run-name`, `run name`, or a name with a non-ASCII character as an identifier
- **THEN** the framework rejects the value instead of creating an identifier

#### Scenario: Compare and hash valid names

- **WHEN** callers construct multiple identifiers from the same valid name and another from a different valid name
- **THEN** equal names compare equal and produce equal hashes, while distinct names remain distinguishable and orderable

### Requirement: Meaningful empty values are distinct from names

The framework SHALL provide `empty_string` and `identifier_or_empty`, containing either that
empty value or a valid identifier. Text, equality, ordering, formatting, and hashing SHALL be
consistent with the value. `suffix_t` SHALL alias `identifier_or_empty` without different validity
or behavior. Empty-capable plugin components SHALL use `identifier_or_empty` directly; actual
plugin names SHALL remain non-empty identifiers.

#### Scenario: Compare an empty value with a name

- **WHEN** a caller compares or hashes an empty value and a non-empty named value
- **THEN** the values remain distinct, equal empty values produce equal hashes, and both values can be ordered and formatted

#### Scenario: Reject an invalid named alternative

- **WHEN** a caller attempts to supply invalid non-empty text as a suffix or plugin-name component
- **THEN** the framework rejects it rather than storing it as an empty value or an invalid identifier

### Requirement: Created product labels have concrete layer names

A complete product specification for a created product SHALL include a present, non-empty,
valid identifier as its data-layer name. An omitted or empty layer SHALL NOT describe a created
product. The representation MAY combine metadata from a product specification, data cell, and
provider; the product-specification object alone need not contain the entire label.

#### Scenario: Name a created product's layer

- **WHEN** a product is associated with the layer `event`
- **THEN** its complete specification has the concrete layer name `event`

#### Scenario: Reject a nameless layer

- **WHEN** a caller attempts to create a data cell or provide a product with an empty or otherwise invalid layer name
- **THEN** the framework rejects the creation rather than recording a nameless layer

### Requirement: Product selector treats an omitted layer as unrestricted

A product selector SHALL accept an omitted layer criterion and match eligible products from
any layer. A supplied layer criterion SHALL be a valid, non-empty identifier and SHALL match
only that layer. The framework SHALL NOT represent an omitted criterion as a concrete layer name.
Existing ambiguity rules for a product selector matching more than one product SHALL remain in
effect.

#### Scenario: Select without naming a layer

- **WHEN** a selector omits its layer and otherwise matches products labeled in distinct layers
- **THEN** products in either layer are eligible matches, subject to the existing ambiguity rules

#### Scenario: Select a particular layer

- **WHEN** a selector specifies `event` as its layer and otherwise matches products in `event` and `job`
- **THEN** only the product in `event` is eligible to match that selector

#### Scenario: Reject an empty selector layer

- **WHEN** a selector configuration supplies `""` as its layer criterion
- **THEN** the framework rejects that selector instead of interpreting the empty string as any layer

#### Scenario: Source provides a concrete layer for an unrestricted selector

- **WHEN** a source supplies a product to satisfy a selector whose layer is omitted
- **THEN** it uses the product's actual, non-empty layer for matching and provider registration, or reports missing concrete layer metadata instead of deriving a layer from the selector

### Requirement: Product specifications have explicit suffixes

A product specification SHALL have an explicit suffix that is either empty or a valid name. If
no output suffix is specified during registration, the resulting product specification SHALL
have an explicit empty suffix.

#### Scenario: Default output suffix is explicitly empty

- **WHEN** an algorithm registers an output without specifying its output suffix
- **THEN** the created product's suffix is explicitly empty, not an omitted selection criterion

### Requirement: Product selector preserves three distinct suffix states

A product selector SHALL distinguish an omitted suffix criterion, which matches any product
suffix; an explicit empty suffix, which matches only empty suffixes; and a named suffix, which
matches only that name. Textual configuration and Python selector input SHALL preserve those
distinctions in matching, comparison, and diagnostic text.

#### Scenario: Match omitted, empty, and named suffixes

- **WHEN** available products have empty and `hits` suffixes
- **THEN** an omitted selector suffix admits both, an explicit `""` admits only the empty suffix, and `hits` admits only the named suffix

#### Scenario: Preserve suffix meaning across selector inputs

- **WHEN** JSON configuration or a Python selector provides no suffix field, an empty string, or `hits`
- **THEN** the resulting product selectors retain the corresponding omitted, explicitly empty, and named matching behavior

### Requirement: Product selector matches one creator name

A product selector's supplied creator criterion SHALL be one valid identifier. It SHALL match
either the plugin name or the algorithm name of a product's creator. An omitted creator
criterion SHALL impose no creator restriction. A qualified creator name SHALL NOT be a valid
selector criterion.

#### Scenario: Match a plugin or an algorithm

- **WHEN** a product has creator `reco:find_hits` and a selector supplies `reco` or `find_hits`
- **THEN** either creator criterion matches that product, subject to the other selection criteria

#### Scenario: Reject a combined creator criterion

- **WHEN** a product selector supplies `reco:find_hits` as its creator criterion
- **THEN** the framework rejects the criterion rather than adding combined-name matching

#### Scenario: Omit a creator criterion

- **WHEN** a product selector omits its creator criterion
- **THEN** creator identity does not restrict its eligible matches

### Requirement: Creator plugin components may be empty

A creator name SHALL have a valid, non-empty algorithm component. Its plugin component MAY be
explicitly empty for an unqualified creator name. A qualified creator name SHALL contain valid,
non-empty plugin and algorithm names. A supplied actual plugin name SHALL be non-empty.
Parsing and formatting SHALL preserve absence without constructing an empty identifier.

#### Scenario: Use an unqualified creator

- **WHEN** a caller specifies the creator `find_hits`
- **THEN** its algorithm component is `find_hits` and its plugin component is explicitly empty

#### Scenario: Use a qualified creator

- **WHEN** a caller specifies the creator `reco:find_hits`
- **THEN** its plugin and algorithm components are the valid names `reco` and `find_hits`

#### Scenario: Reject an invalid creator component

- **WHEN** a caller supplies `:find_hits`, `reco:`, an empty creator name, or an invalid component
- **THEN** the framework rejects that creator name

#### Scenario: Reject an empty actual plugin name

- **WHEN** configuration or registration supplies an empty actual plugin name
- **THEN** the framework rejects it rather than treating the supplied value as an absent component

### Requirement: Product specifications can have absent creators

A product specification SHALL permit an absent creator for exact storage and lookup. Absence
SHALL remain distinct from every present creator in equality, ordering, and hashing. A missing
creator SHALL NOT act as a wildcard. Textual suffix-only specifications SHALL retain this state.
Provider registration SHALL require a present creator.

#### Scenario: Store and find a suffix-only specification

- **WHEN** a caller stores a concrete data product with suffix `hits` and no creator
- **THEN** the same specification finds it, but a specification with a present creator does not

#### Scenario: Parse suffix-only text

- **WHEN** a caller parses `hits`, `/hits`, `""`, or `/` as product-specification text
- **THEN** its creator is absent and its suffix is the corresponding named or explicit empty value

#### Scenario: Require a provider creator

- **WHEN** a caller attempts provider registration with an absent creator
- **THEN** the framework reports the missing creator instead of constructing an empty creator name

### Requirement: Registration and handle interfaces retain agreed boundaries

The fluent output-suffix registration interface SHALL continue accepting textual suffixes,
including the empty string, and SHALL produce validated suffix values in product
specifications. Explicit-provider registration SHALL accept a `suffix_t`, including its empty
alternative. Handles SHALL continue exposing borrowed creator, suffix, layer, and stage names
as textual views, rather than requiring callers to use the new domain types.

#### Scenario: Register an empty fluent output suffix

- **WHEN** a caller supplies `""` to the fluent output-suffix interface
- **THEN** the resulting product specification has an explicit empty suffix

#### Scenario: Register an explicit-provider empty suffix

- **WHEN** a caller supplies the empty `suffix_t` to explicit-provider registration
- **THEN** the provider's product specification has an explicit empty suffix

#### Scenario: Read product metadata through a handle

- **WHEN** a caller inspects a handle for a product with an empty suffix
- **THEN** the handle exposes the suffix as an empty textual view and continues exposing its other names as textual views

#### Scenario: Read a handle with an absent creator

- **WHEN** a caller inspects a handle whose product specification has no creator
- **THEN** its creator-component views are empty and its metadata remains borrowed from the product specification

### Requirement: Python name conversion preserves complete input

Python name conversion SHALL validate the complete supplied text, including embedded NUL
characters. Invalid names SHALL produce a Python error. Concrete provider output metadata
SHALL accept unqualified or qualified creator names independently of product-selector rules.
Generated names used by translators SHALL be valid and SHALL preserve the final output suffix.

#### Scenario: Register a qualified Python provider output

- **WHEN** Python registers a concrete provider output with creator `reco:find_hits`
- **THEN** registration preserves both creator components without applying the one-name selector rule

#### Scenario: Reject complete invalid Python text

- **WHEN** Python supplies an invalid required name or a name containing an embedded NUL character
- **THEN** the framework reports a Python error rather than truncating the text or letting a C++ exception escape

#### Scenario: Preserve an empty translated output suffix

- **WHEN** a Python transform registers an output with an explicit empty suffix
- **THEN** any generated intermediate name is valid and the final concrete data product retains its empty suffix

### Requirement: FORM suffix conversions preserve valid persisted values

The FORM boundary SHALL preserve the textual suffix representation for new and existing
products: an empty persisted suffix SHALL map to an explicit empty suffix and a valid non-empty
persisted suffix SHALL map to its validated name. It SHALL reject an invalid non-empty legacy
suffix with a diagnostic rather than silently rewriting or grandfathering the value. The
persisted file format SHALL remain unchanged.

#### Scenario: Read and write valid or empty suffixes

- **WHEN** FORM reads or writes a product with `""` or `hits` as its suffix
- **THEN** its stored text and reconstructed product suffix preserve the same value

#### Scenario: Reject an invalid legacy suffix

- **WHEN** FORM encounters a persisted non-empty suffix such as `bad-suffix`
- **THEN** the framework reports the invalid value and does not construct a product specification from it

### Requirement: FORM obtains layers from existing index information

FORM SHALL determine a recovered concrete data product's data-layer name from existing index
information for its persisted creator and stage. It SHALL use the name only when that information
establishes one concrete layer. Missing, conflicting, or ambiguous information SHALL cause an
error. A product selector's layer SHALL NOT supply or resolve the recovered layer.

#### Scenario: Recover one established layer

- **WHEN** the relevant FORM index information establishes one data-layer name
- **THEN** FORM uses that name for provider registration and matches any supplied layer criterion against it

#### Scenario: Reject uncertain layer information

- **WHEN** the relevant FORM index information is missing or does not establish one data layer
- **THEN** FORM reports an error even if a product selector supplies a layer name

#### Scenario: Preserve stored and advertised creators

- **WHEN** FORM configuration advertises a creator name different from the persisted creator
- **THEN** FORM uses the persisted creator for index lookup and retains the configured advertised creator for provider registration

### Requirement: Stage-specific reservation does not invalidate generic names

The framework SHALL accept `CURRENT` as a generic identifier and continue allowing it as a
stage placeholder in product selectors and provider registration. It SHALL reject `CURRENT` as the
name of an actual execution stage; actual stage names SHALL satisfy identifier syntax.

#### Scenario: Resolve the current-stage placeholder

- **WHEN** a product selector or provider registration uses `CURRENT` to refer to the running stage
- **THEN** the framework retains its existing placeholder behavior

#### Scenario: Reject a reserved execution-stage name

- **WHEN** a program attempts to run with `CURRENT` as its actual execution-stage name
- **THEN** the framework rejects the name despite its valid generic identifier syntax
