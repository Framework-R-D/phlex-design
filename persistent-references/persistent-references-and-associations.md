# Persistent References and Associations in Phlex

Describing persistent references to, and associations between, data products
(and members of collection-valued data products) in the context of an
interface/data-description language.

- Status: discussion draft for colleague and project-lead review
- Scope: design analysis only; no implementation is proposed here as final
- Repositories surveyed: `phlex`, `phlex-design`, `phlex-examples`,
  `phlex-coding-guidelines`; plus the FORM backend and the `art`/`canvas`
  reference implementation (`../art-devel/canvas`)

## 1. Original brief

> I need to understand how to describe persistent references to, and
> associations between, data products (and members of data products that are
> collections) as described/implemented in the phlex-design, phlex, and
> phlex-examples repositories (including tests). I wish to do this in a general
> manner in the context of an interface/data description language (e.g.
> associations between ParticleTracks and the DetectorHits from which they are
> formed).
>
> Other context:
>
> - <https://github.com/orgs/Framework-R-D/discussions/520>
> - `art::Ptr` and `art::Assns` (and related `FindOne` and `FindMany` query
>   classes) in `../art-devel/canvas` (the IDL in that case is `rootcling` XML,
>   see `canvas-root-io`, etc.)
> - I have no idea if there is an existing general-purpose third-party IDL that
>   will satisfy the Phlex use case (we wish to avoid dependence on ROOT's IDL
>   outside the FORM I/O backends). My perception is that IDLs like Protobuf and
>   Cap'n Proto are very good at describing *specific* data and relationships
>   between them, but not so much data structure *specifications*.

### 1.1 Framing premise (author's)

The following premise is offered by the author as the organizing principle for
this report; it is put forward for discussion, not asserted as an existing
project decision. Phlex is **only the framework**. A complete data model — describing all the
data needed for an experiment's or collaboration's simulation, reconstruction,
and analysis — is a large, complex artifact owned by the combination of an
experiment's own code and third-party systems such as LArSoft, NuTools, GENIE,
and Geant4. As designers and implementors of Phlex, our job is **not** to author
a data model; it is to **provide the tools and protocols** by which other
parties describe their own data model(s) such that Phlex can satisfy their needs
as efficiently as possible within their resource constraints (processing power,
memory, storage, programming expertise, etc.).

This framing is adopted throughout the report. Everything below concerns the
*mechanism Phlex must offer* for users to express references and associations,
not any specific physics data model. It is consistent with the conceptual
design, which states plainly that "Data product types are not defined by the
framework. Framework users are expected to define their own data product types"
(`phlex-design/doc/ch_conceptual_design/data_organization.rst:62-63`).

## 2. Method

Targeted reading of the four repositories (grep/glob to locate, focused reads of
relevant sections), the FORM persistence backend, the `art`/`canvas` reference
implementation, plus discussion #520 and its linked issues #74 and #75. File and
line references are given inline so claims can be checked.

### 2.1 Third-party IDLs considered, and the limits of that consideration

To keep the scope of this report honest, the third-party interface/data
description languages actually examined here are exactly:

- **Protocol Buffers (Protobuf)** and **Cap'n Proto** — the two named in the
  brief (§1);
- **ROOT `rootcling` selection XML / lcgdict** (e.g. `classes_def.xml` +
  `LinkDef.h`) — the IDL `art` and FORM already use, examined in the repositories
  directly.

The following candidates a reader might reasonably expect were **not** evaluated:
FlatBuffers, Apache Thrift, Apache Avro, ASN.1, Simple Binary Encoding (SBE),
JSON Schema, and HDF5's own datatype system used as a description language.

Two further limitations of this consideration must be stated plainly:

1. The assessment of Protobuf and Cap'n Proto in §4 and §5 rests on their
   **published design characteristics** and the author's general knowledge of
   them, **not** on a hands-on prototype, a feature-by-feature trial, or a
   documented comparison matrix against the Phlex criteria of §5.1.
2. Because only two general-purpose IDLs were examined, any conclusion phrased as
   "no mainstream IDL …" is an **extrapolation** beyond the evidence gathered, and
   is qualified accordingly in §4 (conclusion 5).

A more rigorous evaluation against an explicit candidate set and the §5.1 criteria
is the subject of §5.4.

## 3. Evaluation and reasoning

### 3.1 The in-memory identity model already in place

Phlex has a coherent, I/O-format-agnostic notion of product identity. Any
reference mechanism must be built from three coordinates that already exist (or
are clearly implied) in the code:

| Coordinate | Type / source | Role | `art` analog |
| --- | --- | --- | --- |
| Which cell | `phlex::data_cell_index` (`phlex/phlex/model/data_cell_index.hpp`) | Hierarchical address `(layer_name, number)` plus parent chain, e.g. `Job → Run₃ → Spill₅ → APA₉`; carries a precomputed hash | `EventID` `(Run, SubRun, Event)` — but Phlex's is arbitrary-depth and experiment-defined, not a fixed three levels |
| Which product in the cell | `phlex::experimental::product_specification` (`phlex/phlex/model/product_specification.hpp`) | `algorithm_name{plugin, algorithm}` + `suffix` (instance name) + `type_id`; the *label* | `(module label, instance name, process name, type)` compressed to a `ProductID` checksum |
| Which element of the collection | none yet | Index into a collection-valued product | `art::Ptr::key_type` (`std::size_t`) |

`handle<T>` (`phlex/phlex/model/handle.hpp:108-115`) is the in-memory view: a raw
`product*` plus a `data_cell_index*` plus creator/suffix/type. It is
deliberately **not persistable** — it holds raw pointers — which is consistent
with the requirement that the framework "separate the persistent representation
of data products from their in-memory representations as seen by algorithms"
(`phlex-design/doc/appendices/requirements.rst:36`; DUNE 2). The design corollary
is important: **a persistent reference is not a serialized handle.** It is the
identity triple above, from which a handle can be *reconstructed* on read.

The conceptual design already uses subscript notation that is, in effect, the
persistent-reference coordinate: "GoodHits₃,₅,₉ denotes the GoodHits data product
belonging to APA 9 of Spill 5 of Run 3"
(`phlex-design/doc/conceptual_design.rst:66`).

### 3.2 Associations are intended to be data products

The most significant concrete finding is in the mock workflow.
`phlex/test/mock-workflow/types.hpp` defines a placeholder:

```cpp
namespace phlex::experimental {
  template <typename A, typename B, typename D = void>
  struct association {};
}
```

and `phlex/test/mock-workflow/largeant.cpp:12-19` uses it as an **output
product**, in the algorithm's output tuple alongside the collections it relates:

```cpp
using assns =
  phlex::experimental::association<simb::MCParticle, simb::MCTruth, sim::GeneratedParticleInfo>;
using output = std::tuple<sim::ParticleAncestryMap,
                          assns,
                          sim::SimEnergyDeposits,
                          /* ... */,
                          simb::MCParticles>;
```

This is a direct transcription of the `art` model — where `Assns<A, B, D>` is a
product `put()` into the event — into Phlex's functional, tuple-returning
algorithm model. The design intent is therefore: **an association is a
first-class data product `association<A, B, D>` whose elements are pairs of
references with optional per-link data `D`.** This is exactly the ParticleTracks
↔ DetectorHits case: `association<ParticleTrack, DetectorHit, D?>`, where `D`
might carry, e.g., a fit weight for each track-hit link.

### 3.3 The persistence layer already anticipates references and associations

FORM (`phlex/form/`) is the only place ROOT IDL is permitted, and it already
provides the I/O-side primitives a reference mechanism needs:

- `Token` (`phlex/form/core/token.hpp:12-42`) = `(fileName, containerName,
  technology, id/entry-number)` — a fully resolved *persistent address* of a
  stored object. This is the format-specific resolution of a reference and is
  the natural boundary object between a format-agnostic reference and a backend.
- `Placement` (`phlex/form/core/placement.hpp:13-35`) = `(fileName,
  containerName, technology)` — where to write a container.
- `Storage_Write_Association` (`phlex/form/storage/storage_write_association.hpp`)
  — a dedicated container type for associations, already distinct from ordinary
  containers, though presently a thin stub
  (`phlex/form/storage/storage_write_association.cpp`).

ROOT's IDL is confined exactly as desired: the selection XML and Cling directives
live only under test data products,
`phlex/test/form/data_products/classes_def.xml`:

```xml
<lcgdict>
  <class name="TrackStart"/>
  <class name="std::vector<TrackStart>"/>
</lcgdict>
```

with a sibling `phlex/test/form/data_products/extra_member/` that holds the same
selection XML against a `track_start.hpp` carrying an added data member. That
sibling exists to exercise **schema evolution / backward compatibility**
(`phlex/test/form/form_root_schema_read_test.cpp:1` — "can we build a program
against a new dictionary for the same product that reads the old product and
gets back the correct values?"). The repository therefore already treats
*versioned structure specification* as a concern, but exercises it through ROOT
today.

### 3.4 The IDL/DDL itself does not yet exist — this is greenfield

The design chapters that would normatively specify references, associations, and
the data-description mechanism are explicit stubs:

- `phlex-design/doc/ch_subsystem_design/role_of_data_product_concepts.rst` — the
  entire content is a `.. todo::` including "Describe how the IDL and code
  generation supports data product concepts."
- `phlex-design/doc/ch_subsystem_design/data_modeling.rst` — `.. todo::`
  including "Provides the mechanism by which users define data product types …
  connecting in-memory data representations with information needed by the IO
  backends to read-and-write the data."

The discussion and its issues confirm the direction and constrain scope:

- Discussion #520 (the July 2025 developer retreat): decided to *support*
  persistent references à la `art::Ptr<T>`, to *alter allowed algorithm
  signatures* to accommodate them, and to "not pursue [in-memory bare pointers],
  explore [IDL-based translation]".
- Issue #74: "Explore translating persistent references to framework-independent
  input-parameter types" — states verbatim, "This may require use of an
  interface description language (IDL)."
- Issue #75: "Support inter-data-cell persistent references" — scopes the hard
  requirement: a track within a Spill data cell may need to refer to hits that
  belong to different APA data cells; explicitly tracks *design, not
  implementation.*

The conceptual vocabulary already exists in an earlier design note,
`phlex-design/dune-fw-design-ideas-chg.md:50-68`, which defines a **"view"** as
the umbrella concept and lists, among its kinds, "A bi-directional association
between two views, with an optional data group." In other words: a *reference* is
a unidirectional view; an *association* is a bidirectional view with optional
attached data — terminology that maps cleanly onto `art::Ptr` and
`art::Assns<A, B, D>` respectively.

### 3.5 What the examples reveal about the present workaround

The migrated LArSoft Gaussian hit finder
(`phlex-examples/migration/gauss_hit_finder/copied_from_larsoft_minor_edits/Hit.h:9-14`)
documents that porting `recob::Hit` to Phlex required **removing the
`art::Ptr<recob::Wire>` and `art::Ptr<raw::RawDigit>` members** and replacing
them with embedded intrinsic identifiers (`fChannel`, and a `geo::WireID
fWireID` value). That is the present strategy in the absence of a reference
mechanism: encode a *domain* key, not a framework reference.

This is an important design signal. There are two legitimate, co-existing styles
that any Phlex data-description tooling must let users express:

1. **Intrinsic domain key** — a value member that happens to identify another
   datum (e.g. `geo::WireID`). Cheap, self-contained, ROOT-readable as bare
   data, but opaque to the framework (no provenance, no automatic navigation).
2. **Framework reference** — `(ProductID-analog, key, cell)` resolved by the
   framework, providing navigation and provenance but requiring framework or
   generated code to dereference.

The framework should support and not prejudge the choice; the trade-off belongs
to the user and their resource constraints, per the §1.1 framing.

### 3.6 The `art` reference model, distilled (proven prior art)

From `../art-devel/canvas`:

- `art::Ptr<T>` (`canvas/Persistency/Common/Ptr.h`) = a `RefCore` plus
  `key_type key_`. Persistently it reduces to `(ProductID, key)`; the `RefCore`
  also caches a transient `EDProductGetter*` and a resolved pointer
  (`canvas/Persistency/Common/RefCore.h:19,70`). The lesson: **persistent state
  is just `(ProductID, index)`; everything else is transient and reconstructed
  on read.**
- `art::ProductID` (`canvas/Persistency/Provenance/ProductID.h:17-22`) = a single
  `unsigned int` checksum of the canonical product name — a compression of the
  product-label tuple. Notably it does **not** encode the event; a `Ptr`
  resolves within the current event's product lookup.
- `art::Assns<L, R, D>` (`canvas/Persistency/Common/Assns.h:5-80`) = a persistable
  `vector<pair<Ptr<L>, Ptr<R>>>` with an optional parallel `vector<D>`;
  supports one-to-one, one-to-many, and many-to-many. `FindOne`/`FindMany` are
  *query views* layered on top and are **not** stored.

The one structural gap `art` cannot cover for Phlex: `art::Ptr` is implicitly
same-event, whereas Phlex issue #75 requires cross-cell references. So a Phlex
persistent reference must additionally carry a cell coordinate — which can be
omitted/defaulted to "same cell" to keep the common case as cheap as `art`'s.

## 4. Conclusions

1. **Phlex already has the identity substructure** for references:
   `data_cell_index` (which cell), `product_specification` (which product), and a
   missing-but-obvious element index (which collection member). A persistent
   reference is the triple, not a serialized `handle`.

2. **Associations are already modeled as data products** in intent
   (`association<A, B, D>` in `largeant.cpp`), mirroring `art::Assns`. This is the
   right abstraction and should be made real.

3. **The persistence backend already anticipates the mechanism**: `Token`
   provides format-specific addressing, and `Storage_Write_Association` reserves
   a place for association containers. ROOT IDL is, and should remain, an
   *output* of FORM, not an input authored by users.

4. **The normative IDL/DDL does not yet exist** — the relevant design sections
   are stubs, and issue #74 explicitly anticipates needing an IDL. This is
   greenfield design, not reverse-engineering of an existing facility.

5. **The two general-purpose IDLs examined do not satisfy the
   specification-level needs.** Of the IDLs actually considered here (§2.1),
   **Protobuf and Cap'n Proto** describe *specific* messages well but lack
   vocabulary for the *structure specifications* Phlex needs — in particular,
   they have no notion of typed references/associations with cardinality and
   directionality, no transient-vs-persistent member distinction, no
   concept-vs-concrete-type mapping, and only additive schema evolution. This
   confirms the user's stated perception for those two tools. The broader claim
   that *no* mainstream IDL suffices is plausible but **not yet established**: it
   is an extrapolation from two examples, and candidates such as FlatBuffers,
   Thrift, Avro, ASN.1, SBE, and JSON Schema were not evaluated (see §2.1 and the
   recommended follow-up in §5.4).

6. **Per the §1.1 framing, the deliverable is a protocol plus code generation,
   not a data model.** Phlex should publish a small data-description vocabulary
   and a generator that targets the backends; experiments and toolkits (LArSoft,
   NuTools, GENIE, Geant4 wrappers, …) populate it with their own types.

### 4.1 Proposed canonical reference form

```text
phlex_ref  ≔  ⟨ cell_coord?, product_spec_id, element_index? ⟩
```

- `cell_coord` — encodes a `data_cell_index`. **Optional**, defaulting to "the
  cell containing the referrer" (the `art`-compatible, cheap, common case). When
  present, expresses inter-cell references (issue #75). Encode as the layer-path
  with per-layer numbers, e.g. `[(Run,3),(Spill,5),(APA,9)]`, or as the hash that
  `data_cell_index` already precomputes plus a side table.
- `product_spec_id` — the `product_specification` (creator plugin+algorithm,
  suffix, type), inline or as a checksum into a per-file product-spec table (the
  `ProductID` strategy). `product_specification` is already hashable.
- `element_index` — `std::size_t`. **Absent** ⇒ reference to the whole product;
  **present** ⇒ reference to a member of a collection product (the `art::Ptr`
  `key` role). This single optional field is what unifies "reference to a data
  product" and "reference to a member of a collection-valued data product."

An association is then a product:

```text
association<A, B, D>  ≔  collection of ⟨ phlex_ref→A , phlex_ref→B , D? ⟩
```

with `FindOne`/`FindMany`-style **query objects** (foreshadowed by
`phlex-design/product-queries/`) generated as transient navigators and never
persisted.

## 5. Recommendations

### 5.1 What a Phlex data-description vocabulary must express

These are the capabilities general IDLs lack and that the design docs already
call for. They define the minimum vocabulary Phlex must offer its users:

1. **Product identity and concept binding** — map a *data-product concept* to one
   or more *concrete types*, since "A data-product concept can be modeled by
   multiple concrete data-product types"
   (`phlex-design/doc/appendices/glossary.rst:51-55`) and a `Translator` converts
   between concretes that model the same concept
   (`phlex-design/doc/appendices/glossary.rst:115-118`).
2. **Transient vs persistent members** — "Named components … may be flagged as
   transient and therefore not persisted by default"
   (`phlex-design/dune-fw-design-ideas-chg.md:29-30`).
3. **References and associations as typed schema elements** — `ref<T>`,
   `ref_element<T>`, and `association<A, B, D>` with cardinality (1:1 / 1:N / N:M)
   and directionality, expressed in the "view" / "bidirectional view" vocabulary
   (`phlex-design/dune-fw-design-ideas-chg.md:50-68`). This is the crux and the
   precise capability no off-the-shelf IDL provides.
4. **Cell scoping of references** — same-cell (default) vs inter-cell, tied to the
   experiment-defined data-layer hierarchy (issue #75).
5. **Schema evolution / backward compatibility** — defined in the glossary
   (`phlex-design/doc/appendices/glossary.rst:15-16`) and exercised by FORM's
   `extra_member` test; needs read-old/write-new with translators, beyond
   Protobuf's additive-only field rules.
6. **Backend-agnostic with backend hints** — one source of truth from which ROOT
   `classes_def.xml`/`LinkDef.h` (FORM) *and* HDF5 layouts are generated, so
   hand-authored ROOT IDL becomes a generated artifact. Data products "must
   support reading and writing with both ROOT and HDF5"
   (`phlex-design/doc/ch_conceptual_design/data_organization.rst:46`).

### 5.2 Options, with trade-offs

- **Option A — Phlex-native description + code generation (recommended).** Define
  a small declarative schema (most cheaply as JSON/Jsonnet, since the project
  already uses Jsonnet for workflows and Boost.JSON in the model) whose
  vocabulary includes `product`, `concept`, `concrete_type`,
  `member{name, type, transient?}`, `ref<T>`, `ref_element<T>`,
  `association<A, B, D>{cardinality, directional}`, `version`, and per-backend
  hints. Generate from it: (1) C++/Python binding structs, (2) FORM
  `classes_def.xml` + `LinkDef.h`, (3) HDF5 type descriptors, (4) the
  `product_specification`/`type_id` registrations. *Pro:* exactly the
  specification-level expressiveness missing from general IDLs; keeps ROOT IDL
  strictly downstream of FORM; matches the existing "IDL and code generation"
  todo and the §1.1 framing (we ship tooling, users ship the model). *Con:* we
  build and maintain the generator.
- **Option B — Cap'n Proto (or Protobuf) for the value layer, Phlex annotations on
  top.** Describe only leaf/value byte layouts in the third-party IDL; layer
  Phlex concepts (refs, associations, transient flags, concept binding) as
  annotations or an outer schema. *Pro:* reuse a mature serializer for leaf data.
  *Con:* two schema systems to reconcile; the third-party pointer model does not
  match `(ProductID, key, cell)` semantics, so references still live in the Phlex
  layer; transient/concept/evolution semantics still need a Phlex layer anyway.
- **Option C — reuse `art`'s `rootcling` XML as the IDL.** *Rejected* by the
  stated constraint (ROOT IDL must stay inside FORM) and by the dual-backend
  requirement (must also target HDF5). Retain it only as a *generated* artifact.

**Net:** of the IDLs examined here (§2.1), none satisfies Phlex's
specification-level needs, and the references/associations vocabulary in
particular has no equivalent in Protobuf or Cap'n Proto. Subject to the
broader survey recommended in §5.4 not overturning this, Option A is the design
the repositories already point toward (the `IDL and code generation` todo, the
`association<A, B, D>` placeholder, FORM's `Token`/`Storage_Write_Association`,
and the schema-evolution test all converge on it), and it is the option most
consistent with the framing that Phlex provides tools, not the data model.

### 5.3 Suggested next steps for prototyping

1. Fill `role_of_data_product_concepts.rst` and `data_modeling.rst` with the
   reference/association vocabulary above; they are the normative homes and are
   currently empty stubs.
2. Promote `phlex::experimental::association<A, B, D>` from the empty placeholder
   in `phlex/test/mock-workflow/types.hpp` to a real model type whose persistent
   form is `vector<⟨ref, ref, D?⟩>`, and define `ref<T>` / `ref_element<T>`
   carrying the `⟨cell_coord?, product_spec_id, element_index?⟩` triple.
3. Give `Storage_Write_Association` real semantics that map `ref → Token` on
   write and `Token → resolved handle` on read.
4. Write the description schema and a generator emitting `classes_def.xml` /
   `LinkDef.h` for the existing `track_start` / `extra_member` tests, proving the
   round-trip and schema evolution without hand-authored ROOT IDL.

### 5.4 Evaluation of candidate IDLs against the Phlex criteria

This section supplies the explicit comparison that §2.1 flagged as missing. It
evaluates a candidate set against the six capabilities of §5.1. The candidate
set is broader than the two IDLs examined in the body of this report, but the
assessments below are based on the **published design and documented feature
sets** of each tool, not on hands-on prototyping; rows so derived are marked. The
table is offered as a structured starting point for review, not as a settled
verdict — corrections from colleagues with direct experience of these tools are
expected and welcome.

Legend: ✓ = supported natively; ~ = partial / expressible only by convention or
out-of-band annotation; ✗ = not supported. Criteria (from §5.1): **C1** product
identity & concept→concrete binding; **C2** transient vs persistent members;
**C3** typed references/associations with cardinality and direction; **C4** cell
scoping of references; **C5** schema evolution beyond additive; **C6**
backend-agnostic with hints, targeting both ROOT and HDF5.

| Candidate | C1 | C2 | C3 | C4 | C5 | C6 | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Protobuf | ✗ | ~ | ✗ | ✗ | ~ | ~ | `[deprecated]`/custom options give weak transient/annotation hooks; refs only as ad-hoc IDs; additive field-number evolution only. |
| Cap'n Proto | ✗ | ~ | ~ | ✗ | ~ | ~ | Inter-message *pointers* exist but are intra-message-tree, not `(ProductID, key, cell)`; annotations allow conventions; additive evolution. |
| FlatBuffers | ✗ | ~ | ✗ | ✗ | ~ | ~ | Similar profile to Protobuf; in-place access is a strength irrelevant to the specification-level gaps. |
| Apache Thrift | ✗ | ✗ | ✗ | ✗ | ~ | ~ | RPC-oriented; no transient or reference vocabulary. |
| Apache Avro | ✗ | ✗ | ✗ | ✗ | ✓ | ~ | Strong reader/writer-schema resolution (best-in-class C5), but no refs/associations/concept binding. |
| ASN.1 | ~ | ~ | ~ | ✗ | ~ | ~ | Very expressive type system and multiple encodings (C6-friendly in principle); references and "open types" expressible but unidiomatic; high authoring complexity. |
| SBE | ✗ | ✗ | ✗ | ✗ | ~ | ✗ | Fixed-layout, latency-oriented; wrong tool for this problem. |
| JSON Schema | ~ | ~ | ~ | ✗ | ~ | ~ | `$ref`/`$id` express references *within a schema document*, not cross-product/cross-cell data references; rich for validation, weak as a binding/codegen source. |
| ROOT rootcling/lcgdict | ✗ | ~ | ✗ | ✗ | ✓ | ✗ | Examined directly in-repo; excellent ROOT schema evolution (C5), but ROOT-only (fails C6) and must stay inside FORM per project constraint. |
| Phlex-native DDL (Option A) | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | By construction: the vocabulary is defined to cover C1–C6 and to *generate* the backend IDLs above. |

Observations:

- The decisive, recurring gap across **every** third-party candidate is **C3**
  (typed references/associations with cardinality and direction) and **C4** (cell
  scoping). No surveyed tool models a reference as anything richer than an opaque
  integer/string ID, and none has a concept of an experiment-defined hierarchical
  cell address. These are precisely the Phlex-specific semantics
  (`art::Ptr`/`art::Assns` plus issue #75's inter-cell requirement).
- **C1** (concept→concrete binding) and **C2** (transient flag) are also absent or
  convention-only everywhere except the Phlex-native option.
- Where third-party tools are strong — Avro's reader/writer schema resolution
  (C5), Cap'n Proto's in-place access, ROOT's `+`-suffix schema evolution — the
  strengths are orthogonal to the C3/C4 gap. They could still be valuable for the
  **value/leaf layer** under Option B, but they cannot carry the
  reference/association layer.

This evaluation therefore **upgrades the earlier extrapolation to a supported
finding for the surveyed set**: among the candidates listed, none satisfies C3+C4,
and only the Phlex-native vocabulary (Option A) satisfies C1–C6. The residual
risk is an unsurveyed tool or a future extension of one of the above; that risk is
low for C3/C4 because those semantics are domain-specific to event-data
frameworks and have no general-IDL analog, but the claim remains open to revision
if such a tool is identified.

## 6. Key source references

- In-memory identity:
  `phlex/phlex/model/data_cell_index.hpp`,
  `phlex/phlex/model/product_specification.hpp`,
  `phlex/phlex/model/handle.hpp`
- Associations as products:
  `phlex/test/mock-workflow/types.hpp`,
  `phlex/test/mock-workflow/largeant.cpp:12-19`
- Persistence layer:
  `phlex/form/core/token.hpp`,
  `phlex/form/core/placement.hpp`,
  `phlex/form/storage/storage_write_association.{hpp,cpp}`,
  `phlex/form/storage/istorage.hpp`
- ROOT IDL confined to FORM, plus schema evolution:
  `phlex/test/form/data_products/classes_def.xml`,
  `phlex/test/form/data_products/extra_member/`,
  `phlex/test/form/form_root_schema_read_test.cpp`
- Design intent and vocabulary:
  `phlex-design/doc/ch_conceptual_design/data_organization.rst`,
  `phlex-design/doc/conceptual_design.rst:66`,
  `phlex-design/doc/appendices/glossary.rst`,
  `phlex-design/doc/ch_subsystem_design/role_of_data_product_concepts.rst`,
  `phlex-design/doc/ch_subsystem_design/data_modeling.rst`,
  `phlex-design/dune-fw-design-ideas-chg.md:29-30,50-68`,
  `phlex-design/dune-req-009-issues.md:25,39-43`
- Present workaround in user code:
  `phlex-examples/migration/gauss_hit_finder/copied_from_larsoft_minor_edits/Hit.h:9-14`
- Prior art (`art`/`canvas`):
  `../art-devel/canvas/canvas/Persistency/Common/Ptr.h`,
  `../art-devel/canvas/canvas/Persistency/Common/RefCore.h`,
  `../art-devel/canvas/canvas/Persistency/Common/Assns.h`,
  `../art-devel/canvas/canvas/Persistency/Provenance/ProductID.h`
- Discussion and issues:
  Framework-R-D/discussions/520; Framework-R-D/phlex issues #74 and #75
