# ADR 0001: Naming and Product Specification Invariants

Date: 2026-11-16

Status: Accepted; aggregate-class migration and single-point name validation
implemented in the Phlex repository

## Context

Phlex uses small naming value types to connect algorithm registration, product
descriptions, selection, and storage. Their state distinctions are architectural:
a required name, an absent plugin label, an explicitly empty suffix, and an omitted
selection criterion do not mean the same thing. Adapting callers to an incorrect
aggregate invariant preserves the wrong contract rather than fixing the model.

This decision retains the established `identifier` and `identifier_or_empty`
contracts and changes the contracts of `algorithm_name` and
`product_specification`. The latter two types are currently in
`phlex::experimental`. The migration of those two types is described in
[Implementation Status and Evidence](#implementation-status-and-evidence).

The team is ending its OpenSpec experiment because the process was a poor fit.
This ADR preserves the relevant design knowledge independently of that workflow.
Historical plans were reviewed alongside code, tests, and recent commits; their
existence or completion checkboxes were not treated as implementation evidence.
This decision does not remove the historical artifacts or evaluate the process.

An algorithm's registered name identifies the configured graph entity, not
necessarily the programming-language symbol of its callable. For example,
registration may name a transform `hit_finder` while its operator is `find_hits`.
A plugin label is optional, but the registered algorithm name is not.

A product suffix distinguishes outputs, including multiple products of the same
type from one creator. An empty suffix is meaningful. The C++
`product_specification` class carries creator, suffix, and type information; it
does not contain the entire identity or provenance of a concrete data product.
Layer, data-cell association, and stage information are carried elsewhere. The
domain term *product specification* describes that complete identification
context, which may be distributed across a label and its data cell. An unspecified
type makes the class a usable description before type resolution, not a complete
description of a concrete product.

## Decision

### Responsibilities and Relationships

The intended composition is:

```text
identifier                   validated, required non-empty name
empty_string                 meaningful empty textual value
identifier_or_empty          empty_string | identifier
suffix_t                     alias of identifier_or_empty

algorithm_name
  algorithm_                 identifier
  plugin_                    std::optional<identifier>

product_specification
  creator_                   algorithm_name
  suffix_                    suffix_t
  type_id_                   type_id, known or unspecified

product_selector             matching criteria, not an exact product key
```

The primitive types enforce lexical validity. Aggregate types enforce which
components must exist and how textual components are assembled. Selectors
express matching rules. Registration and graph construction enforce contextual
requirements such as name uniqueness and the correspondence between outputs,
suffixes, and types; a valid value alone cannot enforce those relationships.

### `identifier`: Required Names

This contract is retained, not redesigned:

- A usable identifier owns non-empty, case-sensitive text matching
  `[A-Za-z][A-Za-z0-9_]*`. Keywords are allowed; whitespace, punctuation,
  non-ASCII characters, and a leading digit or underscore are not.
- There is no default construction. Textual construction validates and throws
  `std::invalid_argument` for malformed text. The owned text and cached hash
  correspond to the same identifier; accessors do not expose writable text.
- `identifier` has no `empty()` member. A usable identifier is never empty, so
  the question has one answer, and an emptiness test on one could only guard an
  unreachable state. A value that may legitimately be empty is an
  `identifier_or_empty`.
- Failure messages have the form `Invalid identifier "<text>": it must start with
  an ASCII letter and contain only ASCII letters, digits, or underscores`. The
  text is quoted and escaped and is shortened to 64 characters with `...`.
- C strings convert implicitly; `std::string_view` and the owning rvalue-string
  constructor are explicit. C-string callers must supply a non-null,
  null-terminated string. Length-bearing input retains its supplied length,
  so embedded NUL characters are rejected rather than silently truncating it.
- Normal identifier equality checks both hash and text. Ordering is hash-first,
  then text, not alphabetical. Callers must not infer lexical order from it.
- The `_id` literal validates at runtime. `_idq` creates a compile-time validated
  `identifier_query`, whose comparisons use only the cached hash. It is a fast
  query representation, not collision-free textual identity. `_idq` is
  `consteval` and validates its literal with the same `constexpr` grammar check
  that the runtime constructors use, so a malformed query is a compile error.

Validation belongs at construction rather than at every downstream use. A failed
owning-string construction may consume its input because ownership is acquired
before validation; callers must not assume that input is preserved on failure.

### `identifier_or_empty`: Meaningful Empty Values

This contract is also retained:

- The normal states are `empty_string` and a valid `identifier`. Default
  construction produces `empty_string`; non-empty text uses identifier validation.
- Empty text is a value, not absence. Formatting returns `""`, `named()` returns
  null, and its hash is zero. A named value uses its identifier's text and hash.
- Empty values compare equal only to empty values. They precede named values in
  ordering; named comparison follows `identifier`.
- C strings convert implicitly; strings and string views require explicit
  construction. Length-bearing input is not routed through a C-string boundary.
- `suffix_t` is an alias with exactly these semantics. It adds no extra states.

For a selector, an omitted suffix criterion can match any suffix, whereas an
explicit empty suffix selects the empty suffix. Representing both by the same
empty string would lose information. This rationale supports
`identifier_or_empty` for suffixes; it does not require that representation for
an absent plugin label.

### `algorithm_name`: Required Algorithm, Optional Plugin

The accepted target contract is:

- `algorithm_` is a valid, non-empty `identifier`.
- `plugin_` is `std::optional<identifier>`. A disengaged optional means that no
  plugin label is supplied. An engaged optional contains a valid identifier;
  there is no engaged-but-empty plugin state.
- There is no default or algorithm-less name. Typed construction takes valid
  components, using `std::nullopt` for plugin absence.
- Text is either `algorithm` or `plugin_label:algorithm`, with each named
  component obeying identifier syntax. Empty text, `:algorithm`, `plugin:`, and
  multiple colons are invalid. Formatting emits the same grammar.
- The plugin label is not necessarily the label of a loaded module. For an
  algorithm registered by a configured component it is the label that the
  configuration gives that component under `modules` or `sources`; in a declared
  product creator it is any identifier the author chooses.
- Existing implicit text conversions remain. `match()` remains exact equality
  of the whole name; a bare algorithm does not match every plugin-qualified name.

Optionality models genuine absence, while the required identifier prevents a
usable algorithm-less value. Callers inspecting `plugin()` must check engagement
before dereferencing it. Code constructing unqualified names must pass absence,
not manufacture an empty identifier. These types validate names but do not prove
that a named plugin or algorithm is registered or unique.

### `product_specification`: Required Creator and Explicit Suffix

The accepted target contract is:

- `creator_` is a mandatory `algorithm_name`, not an optional. Every usable
  specification names its creator, which may itself be plugin-unqualified.
- `suffix_` remains `identifier_or_empty`. An empty suffix is valid and explicit;
  there is no separate absent-suffix state in an exact specification.
- `type_id_` may be known or unspecified. In particular, `type_id{}` is permitted.
  `set_type` may set, replace, or clear type information without changing creator
  or suffix. There is no one-way type-resolution invariant.
- No default constructor or creator-less constructor is part of the contract.
  A caller needing an absent specification must represent that absence outside
  this class rather than construct an invalid placeholder.
- Text must contain exactly the creator/suffix separation: `creator/suffix`.
  `creator/` represents an empty suffix. Bare text, leading-slash forms, and
  empty text are invalid. Creator and non-empty suffix use their respective
  validation rules, so extra slashes cannot be part of a valid suffix.
- Existing implicit text conversions remain. Parsing supplies an unspecified
  type, and formatting omits type. Text is therefore not lossless serialization
  of a typed specification; parsing its formatted name may change equality.

Examples of the accepted text contract:

| Text | Meaning or Result |
| --- | --- |
| `find_hits/` | Unqualified creator, empty suffix, unspecified type |
| `reco:find_hits/hits` | Qualified creator, named suffix, unspecified type |
| `find_hits` | Invalid: slash required |
| `/hits` | Invalid: creator required |
| `/` or empty text | Invalid: creator required |
| `reco:/hits` | Invalid: algorithm component required |

The mandatory creator is the decision confirmed in this design discussion. It
removes creator-less storage keys and forces callers to supply the intended
creator. Requiring the slash keeps product text structurally distinct from bare
algorithm text even when the product suffix is empty. These are current decisions,
not attributed historical motives for earlier code.

### Identity, Mutation, and Lifetime

Specification equality and hashing include creator, suffix, and type. Unspecified
type is an exact value that differs from known types, not a wildcard. Selector
matching is a separate operation. Equal specifications must hash equally, but
hash equality alone does not imply specification equality. Changing type changes
identity; callers must not mutate identity-bearing values while relying on an
existing ordered or hashed key placement.

Here, *exact* means no omitted-criterion or wildcard interpretation. It does not
replace `type_id`'s existing equality with RTTI identity: its normal equality and
hashing use structural type information, whereas `exact_compare` uses recorded
`std::type_info`. Distinct C++ types may therefore have equal normal `type_id`
values. This ADR does not redesign that distinction.

Normal invariants apply to usable objects and successful copy/move destinations.
Moved-from identifiers, and aggregates containing them, guarantee only safe
destruction. An engaged optional in a move source does not prove its moved-from
identifier is usable. Callers must not inspect, compare, format, or hash those
sources, or rely on self-move preserving a usable name. This retains the existing
leaf policy rather than adding copying moves, source-name retention, or a reset
to an otherwise forbidden empty algorithm/creator.

Malformed text must fail construction rather than produce a usable malformed
object, and every such failure from an algorithm name or its parts is a
`std::invalid_argument`. Character rules are checked once, by `identifier`.
`algorithm_name` reports the structural errors itself (empty text, a missing
component, more than one colon) and re-reports an identifier failure with the
composed name, the failing part, and the rule, so that the message does not
depend on which entry point received the text. Only the first problem found is
reported. Registration, text conversion, configuration, and the Python and FORM
boundaries all reach the same `algorithm_name::create` overload that takes an
optional plugin label and an algorithm. Boundaries that know which field held
the text, such as a selector's `creator`, `layer`, or `suffix`, or a source's
`creator` parameter, add that field name to the message. A configured module or
source label is checked when the entry is loaded, which lets the message name the
`modules` or `sources` section. The exception types of other aggregates, such as
the structural errors of `product_specification` text, remain outside this
decision. No blanket transactional assignment or allocation-failure guarantee is
introduced. In particular, the `identifier_or_empty` constructor
that copies an identifier is not `noexcept`, because copying the identifier's
string may allocate and throw. The constructor that moves an identifier is
`noexcept`, because moving an identifier does not allocate. Callers supplying
components need usable values, not already moved-from identifiers.

## Implementation Status and Evidence

The implementation snapshot reviewed when the decisions were accepted was Phlex
commit `59e8aa7` on 2026-11-16. The aggregate decisions below were then
implemented on 2026-11-16 on top of that commit, as Phlex commits `f0abfb1`
(optional plugin label) and `81bf0df` (required product creator).

| Area | Before Migration | Accepted Change (Status) |
| --- | --- | --- |
| Identifier leaves | Validated; explicit empty | Retain (unchanged) |
| Algorithm | Required identifier | Retain (unchanged) |
| Plugin | Empty-or-named | Optional identifier (done) |
| Product creator | Optional name | Required name (done) |
| Construction | Default, creator-less | Remove (done) |
| Product text | Bare suffix, `/suffix` | Creator/slash required (done) |
| Suffix and type | Empty suffix; unspecified | Retain (unchanged) |
| Identity | All fields; exact | Retain (unchanged) |
| Type mutation | Set, replace, clear | Retain (unchanged) |

Verification of the migration: the project build and the entire CTest suite
(163 tests) passed through `scripts/verify.sh --project phlex-work-dir`, and
changed files satisfy the repository's clang-format configuration. Tests added
or migrated cover optional plugin states, rejection of `:algorithm`, `plugin:`,
and multiple-colon algorithm text, rejection of bare, leading-slash, and empty
product text, the `creator/` empty-suffix form, absence of default and
creator-less construction, type set/replace/clear, typed versus unspecified
equality and hashing, selector matching against creator, suffix, and type, and
the caller migrations. The remaining language in this section that describes
the earlier implementation is retained as history.

Validation of algorithm names was then made uniform in Phlex commit `09782e4`,
on top of `81bf0df`. Before it, the algorithm parser's character precheck
(`[A-Za-z0-9_[]]`) was more permissive than the leaf validator, so `_x`, `1p:a`,
and `a[0]` passed the precheck and failed at `identifier` construction with a
different exception type and an unrelated message. The change deletes the
precheck, adds the optional-plugin-label `create` overload, throws
`std::invalid_argument` for every malformed name, removes `identifier::empty()`
and the unreachable checks that used it, and adds field names at the selector,
provider-output, and `form_source` parse boundaries. A configured module or
source label is now checked when it is loaded. Its verification is the project
build and the entire CTest suite (165 tests) through the same project build,
with changed files satisfying the repository's clang-format configuration. The
`mypy` hook reports `Module "phlex" has no attribute "Variant"` for
`test/python/pyprovide.py`; that error is unchanged by the commit.

Recent implementation history establishes the following distinctions:

- `29d851a` (2026-11-13) implemented strict leaf validation and the earlier
  aggregate representation: empty-capable plugin and optional product creator.
  Those representations are implemented alternatives now superseded by this ADR,
  not merely abandoned proposals.
- `1a2cdd9` (2026-11-03) introduced the explicit empty-value types before strict
  leaf validation was enabled. The representations and lexical validation were
  separate implementation steps, not one inferred completion event.
- `fa4b1c1` (2026-06-01) replaced partial algorithm-name matching with exact
  equality to make matching symmetric. That formerly implemented wildcard
  behavior is not part of the retained name contract.
- `7807050` (2026-11-14) replaced copying/source-preserving moves with default
  moves and the destruction-only moved-from policy. This ADR retains that policy.
- `2eb425b` and `952a261` (2026-11-14) added coverage of creator-less product
  behavior and its collaborators. Existing tests for those states document the
  old implementation and must change; they do not override the accepted contract.
- `59e8aa7` (2026-11-16) permits optional `value()` calls in clang-tidy. Suppressing
  that warning is not proof that an optional is engaged or safe to dereference.

The relevant implementation lives under the workspace's `srcs/phlex/` symbolic
link. Useful entry points, all relative to that repository, are:

- `phlex/model/identifier.hpp` and `.cpp`: lexical validation, value states,
  construction policies, hashing, query literals, and moved-from documentation.
- `phlex/model/algorithm_name.hpp` and `.cpp`: aggregate representation, parsing,
  formatting, and exact matching.
- `phlex/model/product_specification.hpp` and `.cpp`: specification state,
  parsing, equality/hash fields, type mutation, and output packaging.
- `phlex/model/type_id.hpp`: unspecified state, structural comparison, RTTI
  comparison, and type inference.
- `test/identifier_test.cpp`, `test/core_misc_test.cpp`, and
  `test/product_store_test.cpp`: existing construction and identity expectations.

The architectural context is also described in the existing
[registration chapter](../doc/ch_conceptual_design/registration.rst),
[data organization chapter](../doc/ch_conceptual_design/data_organization.rst),
and [domain glossary](../GLOSSARY.md). The registration chapter's suggestion
that an omitted selector suffix is equivalent to empty conflicts with the current
selector model; it must not erase the distinction recorded here. These references
provide supporting evidence, not prerequisites to understand this decision.

## Consequences and Collaborator Obligations

- Configuration-free registration constructs an unqualified name with
  `std::nullopt`. Configured registration supplies a valid plugin identifier.
  Algorithm names remain required in both paths.
- Creator access becomes unconditional. Handle projection and selector matching
  must stop accommodating an absent specification creator, while still handling
  an absent plugin label and optional selector criteria correctly.
- Source, provider, accumulator, and storage code that used default or
  creator-less specifications must supply intentional creators; in the
  migration, production callers already did, and tests were updated. A fabricated
  universal creator is not implied by this ADR; each caller must preserve its
  actual identity semantics.
- A provider's registration name and its advertised product creator are not
  automatically identical. Explicit registration already accepts them separately;
  implicit provider construction derives its node name from the creator. The
  mandatory-creator invariant does not collapse these roles or resolve graph-name
  collisions.
- Implicit provider construction must read or copy the creator before moving its
  specification. Removing optionality does not make access to a moved-from
  creator safe; the current brace-initialized delegation preserves that ordering.
- Output packaging keeps its existing empty-suffix default and inferred output
  types. The registration API must ensure the number of supplied suffixes agrees
  with the outputs; the packaging helper currently asserts the length agreement.
- APIs requiring exact RTTI comparison or a concrete type name must ensure that
  `type_id` carries usable RTTI metadata. A default unspecified type does not.
  A well-formed specification is not proof of concrete-type completeness.
- Constructors, accessors, hashing, parsers, callers, and tests must be migrated
  together. Creator-less text and bare suffix keys are intentionally not retained
  through compatibility constructors. Persisted-data or external-consumer
  compatibility, if demonstrated, requires a separate decision, not weakening
  these in-memory invariants.

The inspected FORM writer takes creator text from the store source, suffix text
from each specification, and RTTI from the actual payload. It does not persist
the specification's formatted text or numeric hash in that path. FORM's physical
container naming uses `technology__creator__stage/label`; its separator comments
explicitly require a layout version for separator changes. The accepted name and
suffix spelling remains unchanged for valid named values, so the inspected path
does not demonstrate a required storage-layout change. That limited observation
does not prove universal persistence compatibility or stable numeric hashes.
The enforcement points are `form/form_module.cpp` and
`form/core/container_naming.hpp` in the implementation repository.

## Alternatives and Limits

Using `identifier_or_empty` for a plugin label would retain the implemented
representation but model absence as a meaningful empty value. It is rejected for
this component; it remains appropriate for suffixes. Making algorithms or product
creators empty-capable would retain convenient placeholders but violate the
required-name decisions. Callers must model missing objects separately.

Requiring a named suffix or a known type would make specifications more complete
but exclude valid empty-suffix products and descriptions awaiting type resolution.
Those restrictions were considered and rejected. Treating unspecified type as a
wildcard would conflate exact storage identity with selection and is also rejected.
Bare-creator shorthand for an empty product suffix was considered; explicit slash
syntax was selected instead. Adding layer or provenance fields to this class was
not necessary to establish the accepted invariants.

This ADR does not decide the full uniqueness scope of registered names, exact
versus structural type matching policy for every consumer, persistent-label
encoding, or aggregate exception-safety improvements. Historical translator
planning proposed retaining input-product creator metadata while recording
translator identity separately; the reviewed plan alone does not prove that
behavior was implemented. Translator placement, provenance, and selector
multiple-match/layer policies remain outside this class-level decision.

The verification summarized under Implementation Status and Evidence is the
only build and test evidence this ADR asserts. Future changes must continue to
demonstrate the accepted construction and grammar contracts, exact key behavior,
and collaborator behavior against the implementation, not against historical
planning status.
