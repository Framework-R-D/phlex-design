# Phlex–art Comparison Report: Persistent References, Algorithm Migration, and Design Directions

## Executive Summary

This report addresses four questions posed to the Phlex design team regarding persistent
references and LArSoft algorithm migration:

1. **Q1 — Reference type design:** How do art/Phlex architectural differences shape a
   Phlex-native C++ persistent-reference type? Three candidate shapes are analyzed —
   a value-type `ref<T>` (Option A), split `product_ref<T>`/`element_ref<T>` (Option B),
   and framework-pre-resolved inputs with no `ref<T>` in algorithm signatures (Option C).
   The design triple `⟨cell_coord?, product_spec_id, element_index?⟩` maps cleanly onto
   existing Phlex model types for two of three fields; the critical undesigned piece is
   the algorithm-signature change mandated by discussion #520. No single option is
   recommended here; the choice is explicitly for the design team.

2. **Q2 — Algorithm use of references:** art's reference lifecycle (PtrMaker → Assns →
   FindManyP) is fully characterized. Phlex currently has no equivalent — associations
   were dropped entirely in the gauss\_hit\_finder migration (`README.md:142-146`). A
   candidate lifecycle sketch is provided as an illustrative target, not a normative
   proposal.

3. **Q3 — In-art migration categories:** Six categories of in-art, data-product-preserving,
   verifiability-retaining changes are identified: hoisting reference resolution to
   boundaries, isolating framework code behind thin adapters, pre-gathering plain-C++
   inputs, replacing `art::Ptr` data members with intrinsic keys, introducing golden-file
   harnesses, and decomposing multi-output modules.

4. **Q4 — Remaining Phlex migration steps:** Six ordered steps remain after in-art
   preparation, of which only Steps 1 and 2 are actionable today. Steps 3 and 4
   (reference type and FORM persistence) are blocked on five open design decisions
   enumerated at the end of the report.

**Headline finding:** The Phlex model layer already provides two of the three triple
components (`data_cell_index`, `product_specification`). The `association<A,B,D>`
placeholder exists in the mock-workflow. The blocking gap is the algorithm-facing C++
surface — specifically, what type algorithms declare in their signatures and what
machinery mints and resolves references. This is an open design decision; the report
characterizes the options and their trade-offs.

---

## Scope and Methodology

**Questions addressed:** The four questions stated in the plan: (Q1) how art/Phlex
differences should shape a Phlex-native reference type; (Q2) what algorithm use of
references looks like in Phlex and how much machinery users must touch; (Q3) categories
of in-art, data-product-preserving, verifiable migration changes; (Q4) remaining steps
after in-art preparation.

**Sources read locally:**

- `phlex-design/`: `persistent-references-and-associations.md` (full),
  `doc/conceptual_design.rst`, `doc/ch_conceptual_design/data_organization.rst`,
  `doc/ch_technical_design/role_of_data_product_concepts.rst` (todo stub),
  `doc/ch_technical_design/data_modeling.rst` (todo stub), `AGENTS.md`,
  `design_agreements.md`
- `phlex/phlex/model/`: `data_cell_index.{hpp,cpp}`, `product_specification.{hpp,cpp}`,
  `handle.hpp`, `products.{hpp,cpp}`, `product_store.{hpp,cpp}`, `identifier.{hpp,cpp}`,
  `data_layer_hierarchy.{hpp,cpp}`, `layer_path.{hpp,cpp}`, `type_id.hpp`, `fwd.hpp`
- `phlex/test/mock-workflow/`: `types.hpp`, `largeant.cpp`, `ion_and_scint.cpp`,
  `three_tuple_algorithm.cpp`, `algorithm.hpp`, `MC_truth_algorithm.cpp`
- `phlex/form/`: `token.hpp`, `storage_write_association.{hpp,cpp}`, related stubs
- `phlex-examples/migration/gauss_hit_finder/`: full migration set including
  `find_hits_with_gaussians.{hpp,cpp}`, all `register_*.cpp` variants,
  `copied_from_larsoft_minor_edits/Hit.h`, `compare_hits.py`, golden files
- `duneana/`, `ubana/`, `duneanaobj/`: grepped for `art::Assns`, `art::FindManyP`,
  `art::FindOneP`, `art::Ptr`; highest-density files read

**Sources fetched from GitHub:**

- `canvas`: `Ptr.h`, `PtrVector.h`, `Assns.h`, `FindMany.h.in`, `FindOne.h.in`,
  `ProductID.h` (all from `art-framework-suite/canvas`, branch `develop`)
- `art`: `Event.h`, `Handle.h`, `PtrMaker.h` (from `art-framework-suite/art`,
  branch `develop`)
- `larreco`: `GausHitFinder_module.cc` (canonical producer reference)

**Normative vs. exploratory framing:** Per `AGENTS.md`, the authoritative design sources
are files under `doc/` and `design_agreements.md`. Root-level documents including
`persistent-references-and-associations.md` are explicitly non-normative — they are
research analysis and discussion drafts. This report treats them accordingly: the
design triple from `persistent-references-and-associations.md` is cited as a well-grounded
proposal, not as a settled normative decision.

---

## Q1 — How Art/Phlex Differences Should Shape a Phlex-Native Persistent-Reference Type

### Key Art/Phlex Differences Relevant to Reference Design

#### 1. Functional operators vs. imperative module classes

Art algorithms inherit from `EDProducer`/`EDAnalyzer` and interact with the event object
imperatively: `evt.put(...)`, `evt.getValidHandle<T>(tag)`. The event object is a mutable
ambient context. Phlex algorithms are pure functions with explicit I/O in their signatures
(`find_hits_with_gaussians.cpp:98-108`); the framework routes data products by type and
registration metadata. This has a direct consequence for references: in art,
`art::Ptr<T>` is constructed during algorithm execution using `PtrMaker<T>{evt}`, which
requires the event object as a constructor argument (`PtrMaker.h`). A Phlex-native
reference type cannot take a `product_store` or equivalent as a construction-time argument
inside algorithm body code — the reference identity must be computable from the
algorithm's declared outputs alone, or the framework must mint references on the
algorithm's behalf.

#### 2. Framework-owned product lifetime and pure-function semantics

Because Phlex algorithms are pure operators to higher-order functions (transform/fold/unfold),
they do not manage product lifetimes. A `handle<T>` is explicitly non-persistable
(`fwd.hpp:11`) and non-owning (`handle.hpp:46`); it exists only within a
framework-managed execution context. Reference *resolution* — turning a persistent
identity triple into a `handle<T>` — must therefore go through the framework, not through
user-managed pointer caches. This rules out art's pattern of caching a raw pointer inside
`art::Ptr`'s `RefCore` transient field: Phlex has no equivalent transient caching layer
visible to algorithms.

#### 3. Arbitrary-depth data-layer hierarchy vs. art's fixed Run/SubRun/Event

Art's `ProductID` is computed from a fixed four-tuple (type, module label, instance name,
process name) within a fixed event context (`ProductID.h`). Phlex cells are identified by
`data_cell_index` (`data_cell_index.hpp:20`), which encodes an arbitrary-depth
parent/layer hierarchy. The reference triple therefore carries an *optional* `cell_coord`
field mapping to `data_cell_index`: absent means same-cell reference (cheap, no cross-cell
lookup); present means inter-cell reference (`persistent-references-and-associations.md:211-213`,
`AGENTS.md:69-70`). Issue #75 motivates this explicitly: a track in a Spill data cell may
reference hits belonging to different APA data cells. Art has no analog because its
hierarchy depth is fixed at three.

#### 4. Indexed families and unfold

Phlex's `unfold` operator creates *families* of data cells — an unordered indexed set,
not a sequence. A reference to a product in an unfold-created cell must identify that cell
by its `data_cell_index`. The design as currently described in
`persistent-references-and-associations.md` does not yet address how references into
unfold-generated families are resolved when the consuming algorithm executes before the
referenced cell's product is finalized. The triple structure is necessary but not
sufficient for this case.

#### 5. Concept-vs-concrete and Translators

Phlex distinguishes the *concept* of a data product from its *concrete representation*.
A persistent reference should identify the *concept* — specifically the
`product_specification`, which encodes creator plugin, algorithm, suffix, and `type_id`
(`product_specification.hpp:17`) — not the concrete layout. This ensures a reference
written against one concrete remains valid when read back through a Translator into a
different concrete. The IDL/code-generator path recommended in
`persistent-references-and-associations.md` operationalizes this: ROOT IDL becomes a
generated artifact from the concept description, never hand-authored.

#### 6. Framework does not define product types; IDL is needed

If a user-defined product type contains a reference field, the reference's identity
triple must be serializable by FORM and round-trippable through the `Token` mapping
(`token.hpp:12`). Without an IDL or code generator, users would have to manually author
ROOT reflection entries for reference-bearing types. Issue #74 anticipates this
requirement (`persistent-references-and-associations.md:207-209`).

---

### Reconciling the Design Triple with Phlex Model Constructs

The design triple from `persistent-references-and-associations.md:311` is:

```text
phlex_ref  ≔  ⟨ cell_coord?, product_spec_id, element_index? ⟩
```

Each field maps to an existing or anticipated construct:

| Triple field | Phlex construct | Status |
| --- | --- | --- |
| `cell_coord?` | `data_cell_index` (`data_cell_index.hpp:20`) | Exists; arbitrary-depth |
| `product_spec_id` | `product_specification` (`product_specification.hpp:17`) | Exists; creator+suffix+type_id |
| `element_index?` | `std::size_t` (the `art::Ptr::key` role) | No framework type yet |

Two additional constructs bound the in-memory and I/O-layer representations:

- **`handle<T>`** (`handle.hpp:46`): the resolved, in-memory form. Non-persistable,
  non-owning, non-nullable. It is the *output* of reference resolution, never stored in
  a data product. Art analog: `art::Handle<T>` but always valid and always const.
- **FORM `Token`** (`token.hpp:12`): the I/O-layer address (fileName, containerName,
  technology, id). A `phlex_ref` triple must be mappable to/from a `Token` during write
  and read. This mapping does not yet exist; `Storage_Write_Association` is a no-op stub
  (`storage_write_association.hpp:12`).

Per `persistent-references-and-associations.md:111-112`: "A persistent reference is not
a serialized handle. It is the identity triple above, from which a handle can be
reconstructed on read." The `handle<T>` is always a transient, framework-constructed
view; the triple is what persists.

---

### Candidate C++ Shapes

The following three options are for the design team's consideration. None is a settled
decision; all require normative specification to fill the stubs in `data_modeling.rst`
and `role_of_data_product_concepts.rst`.

#### Option A: Value-type `ref<T>`

```cpp
template <typename T>
class ref {
    std::optional<data_cell_index_ptr> cell_coord;    // absent = same-cell ref
    product_specification               product_spec;
    std::optional<std::size_t>          element_index; // absent = whole product
public:
    handle<T> resolve(product_store const&) const;
};
```

**Trade-offs:**

- *Pro:* Clean value semantics; `ref<T>` can appear in output product types (stored in
  `association<A,B,D>` pairs, forwarded across algorithm boundaries). Directly models the
  triple. Decouples identity from I/O layer.
- *Pro:* The `optional<element_index>` cleanly encodes the whole-product vs.
  collection-element distinction.
- *Con:* `resolve(product_store const&)` works naturally for same-cell references. For
  cross-cell references, the framework must maintain a registry of product stores across
  cells and pass it to the resolving algorithm — this registry mechanism is not yet
  designed.
- *Con:* Algorithm signatures must explicitly declare that they consume a `ref<T>`,
  requiring changes to `product_selector` and `input_family` registration not yet
  specified (discussion #520, `AGENTS.md:55-57`).
- *Con:* The `optional<element_index>` introduces a semantic ambiguity: whether `T`
  should be the element type or the collection type must be resolved by convention or
  by separate types (see Option B).

#### Option B: Split `product_ref<T>` and `element_ref<T>`

This option factors the optional `element_index` into two distinct types, analogous to
art's distinction between `art::Handle<vector<T>>` (whole collection) and `art::Ptr<T>`
(single element):

```cpp
template <typename T>
class product_ref {
    std::optional<data_cell_index_ptr> cell_coord;
    product_specification               product_spec;
    handle<T> resolve(product_store const&) const;
};

template <typename T>  // T = element type, not collection type
class element_ref {
    std::optional<data_cell_index_ptr> cell_coord;
    product_specification               product_spec;
    std::size_t                         element_index;
    handle<T> resolve(product_store const&) const;
};
```

**Trade-offs:**

- *Pro:* Eliminates the `optional<element_index>` ambiguity. Type system enforces whether
  a reference is to a whole product or a collection element. Art users familiar with
  `Handle<T>` vs `Ptr<T>` will find the distinction recognizable.
- *Pro:* `element_ref<Hit>` makes the `art::Ptr<Hit>` role explicit; association pairs
  become `pair<element_ref<Wire>, element_ref<Hit>>` with clear semantics.
- *Con:* Two types to teach, document, and register. Code generators must emit both.
- *Con:* Same cross-cell resolution complexity as Option A.
- *Con:* If a product is not a collection, `element_ref` is inapplicable; the framework
  must enforce this constraint at registration time or runtime.

#### Option C: Opaque token resolved by the framework before algorithm dispatch

Under this option, the framework resolves all reference inputs declared in an algorithm's
registration before calling `execute()`, so the algorithm receives `handle<T>` directly.
No `ref<T>` type appears in algorithm signatures.

```cpp
// Registration (illustrative — not yet designed):
m.transform("find_hits", &algorithm_t::find_hits, cfg)
 .input_family(product_selector{...})
 .input_association<recob::Wire, recob::Hit>(assns_label)
 .output_product_suffixes("hits");

// Algorithm signature — algorithm author sees only handles:
std::vector<recob::Hit> find_hits(
    std::vector<recob::Wire> const& wires,
    phlex::query::many<recob::Wire> const& wires_for_hit, ...);
```

**Trade-offs:**

- *Pro:* Algorithm author writes a pure function with no `ref<T>` types, no `resolve()`
  calls, no `product_store` access. Closest to Phlex's "algorithms are pure operators"
  philosophy. Lowest user-facing burden.
- *Pro:* The framework can optimize resolution (e.g., batch cross-cell lookups) without
  algorithm code changes.
- *Con:* A `handle<T>` cannot be stored in an output data product (`fwd.hpp:11`). If an
  algorithm needs to *output* a reference, it cannot use `handle<T>` as the stored form —
  the framework would need a separate API for minting output references, re-introducing
  `ref<T>` or equivalent at the boundary.
- *Con:* Registration API becomes significantly more complex; every reference relationship
  must be expressible in `.input_association(...)`. The current DSL
  (`algorithm.hpp:76-78`) does not support this.
- *Con:* For algorithms that forward references from input to output, Option C has no
  clean model.

---

### Design Implications

The following are offered as considerations for the design team, not mandates:

1. **The triple structure is well-grounded.** The decomposition maps cleanly onto existing
   Phlex model types for two of three fields; the gap is `element_index`.
2. **The same-cell/cross-cell split is architecturally correct.** Same-cell references are
   cheap. Cross-cell references require a framework-maintained registry; this is the
   critical undesigned piece for inter-cell resolution.
3. **The IDL/code-generator path keeps ROOT IDL as a generated artifact.** This is
   consistent with confining hand-authored `classes_def.xml`/`LinkDef.h` to
   `phlex/test/form/data_products/`. The team should evaluate whether the
   `persistent-references-and-associations.md` recommendation should be elevated to
   normative status.
4. **Algorithm-signature changes from discussion #520 are the critical undesigned piece.**
   Until `data_modeling.rst` and `role_of_data_product_concepts.rst` stubs are filled,
   the choice among Options A, B, and C cannot be resolved.
5. **The FORM Token mapping must be specified.** Regardless of which C++ shape is chosen,
   the mapping from `phlex_ref` triple to FORM `Token` (`token.hpp:12`) and the
   serialization logic in `Storage_Write_Association` (`storage_write_association.hpp:12`)
   must be designed before any reference-carrying data product can round-trip through
    storage.

---

## Q2 — Algorithm Use of Persistent References: Art vs. Phlex

### 2.1 Art Reference Lifecycle

Art's reference machinery involves four cooperating components: `art::Ptr<T>`,
`art::Assns<L,R,D>`, `art::PtrMaker<T>`, and the `FindManyP`/`FindOneP` query helpers.

#### Producer side

The producer module declares its output products in its constructor via `produces<recob::Hit>()`
and `produces<art::Assns<recob::Wire, recob::Hit>>()`. In `produce(art::Event& evt)`, it
constructs a `PtrMaker<T>` against the event object — this step computes the future
`ProductID` for the not-yet-`put` collection:

```cpp
// ShowerMerger_module.cc:208-212
art::PtrMaker<recob::PFParticle> PFParticlePtrMaker(e);
auto PFParticle_v   = std::make_unique<std::vector<recob::PFParticle>>();
auto PFP_Vtx_assn_v = std::make_unique<art::Assns<recob::PFParticle, recob::Vertex>>();
```

After populating the collection, the producer mints a `Ptr` to a specific element:

```cpp
// ShowerMerger_module.cc:508-509
PFParticle_v->emplace_back(...);
art::Ptr<recob::PFParticle> const PFParticlePtr =
    PFParticlePtrMaker(PFParticle_v->size() - 1);
```

The `Ptr` is then inserted into one or more `Assns` objects via `addSingle`:

```cpp
// ShowerMerger_module.cc:513-527
PFP_Vtx_assn_v->addSingle(PFParticlePtr, VtxPtr);
```

`art::Assns<L,R,D>` stores pairs as `vector<pair<RefCore, size_t>>` for each side
(`Assns.h`): `ptr_data_1_` holds `(ProductID, key)` for L, `ptr_data_2_` for R. The raw
pointer cached in `RefCore` is transient and not written to disk. Finally,
`evt.put(std::move(PFParticle_v))` and `evt.put(std::move(PFP_Vtx_assn_v))` persist
both products.

#### Consumer side

The consumer constructs a `FindManyP` (or `FindOneP`) helper against a handle and the
event:

```cpp
// AnalysisTree_module.cc:5181-5196
art::FindManyP<recob::Hit> fmht(trackListHandle, evt, fTrackModuleLabel);
std::vector<art::Ptr<recob::Hit>> allHits = fmht.at(iTrk);
```

`FindManyP` is a transient query view built at read time from the `Assns` product; it is
never persisted. `.at(index)` returns `vector<Ptr<Hit>>`. For single-result lookups:

```cpp
// AnalysisTree_module.cc:4840-4848
art::FindOneP<anab::MVAPIDResult> fmvapid(showerHandle, evt, label);
auto pid = fmvapid.at(iShower);
```

#### Art machinery summary

Every step touches art-namespace infrastructure: `art::Event&` as ambient context,
`art::PtrMaker<T>` for minting, `art::Assns<L,R,D>` for storage,
`art::FindManyP`/`FindOneP` for querying, `art::Ptr<T>` as the navigable reference type,
and `produces<>`/`consumes<>()` in module class constructors. The module class inherits
from `art::EDProducer` or `art::EDAnalyzer`.

---

### 2.2 Current Phlex State — No Reference Mechanism

Phlex does not currently have an equivalent to `art::Ptr`, `art::Assns`, `FindManyP`, or
`FindOneP`. This is not a partial implementation — no reference resolution mechanism
exists in the framework.

The `gauss_hit_finder` migration documents the consequence directly: the wire-to-hit and
rawdigit-to-hit associations that `GausHitFinder` produces in art were dropped entirely
in the Phlex migration (`README.md:142-146`). The algorithm signature reflects this:

```cpp
// find_hits_with_gaussians.hpp:58-62
std::vector<recob::Hit> find_hits_with_gaussians(
    find_hits_with_gaussians_cfg const& cfg,
    std::vector<recob::Wire> const& wires, ...);
```

The output is a plain `vector<recob::Hit>` — no `Assns`, no `ref<T>`. The WireID field
in the migrated `Hit.h` is always default-constructed (`find_hits_with_gaussians.cpp:359-365`)
because the geometry service is not yet available in Phlex.

The workaround is to embed intrinsic domain keys in the product type itself
(`Hit.h:52-73`): `raw::ChannelID_t fChannel` and `geo::WireID fWireID` replace the
removed `art::Ptr<Wire>` and `art::Ptr<RawDigit>` (`copied_from_larsoft_minor_edits/Hit.h:10-13`).
These are plain detector-geometry scalars with no framework dependency and no
framework-managed validity guarantee.

The `association<A,B,D>` type exists as an empty placeholder (`types.hpp:5-6`). It
appears in the `largeant.cpp:12-19` mock-workflow algorithm signature as an output type,
but the struct has no stored pairs, no `add()` method, and no serialization support in
FORM (`storage_write_association.hpp:12` is a no-op stub). Its presence documents the
*intent* that associations will be first-class output products; it does not constitute a
working mechanism.

---

### 2.3 What a Phlex Reference Lifecycle Would Look Like — Candidate Sketch

The following is an illustrative sketch based on existing model constructs and the
`phlex_ref` triple design. It is **not yet designed** and does not represent a normative
proposal.

#### Candidate producer side

An algorithm that produces an association would declare it as part of its output tuple,
consistent with the existing mock-workflow pattern (`largeant.cpp:12-19`):

```cpp
// Illustrative — not yet implemented
using output_t = std::tuple<std::vector<recob::Hit>,
                            phlex::association<recob::Wire, recob::Hit>>;

output_t find_hits(std::vector<recob::Wire> const& wires, ...) {
    auto hits  = std::vector<recob::Hit>{};
    auto assns = phlex::association<recob::Wire, recob::Hit>{};
    for (std::size_t i = 0; i < wires.size(); ++i) {
        hits.push_back(make_hit(wires[i]));
        // ref_to() would mint a phlex_ref triple — API not yet designed
        assns.add(ref_to(wires[i], i), ref_to(hits, hits.size() - 1));
    }
    return {std::move(hits), std::move(assns)};
}
```

The key open question: in art, `PtrMaker<T>{evt}` queries the event for the future
`ProductID`. In Phlex, the `product_specification` is derivable from registration
metadata — so a framework-provided minting helper could construct the triple without an
event object. This helper does not yet exist.

#### Consumer side — Option C (framework pre-resolves)

```cpp
// Illustrative — Option C
void analyze(std::vector<recob::Hit> const& hits,
             phlex::query::many<recob::Wire> const& wires_for_hit) {
    for (std::size_t i = 0; i < hits.size(); ++i) {
        auto const& wire_handles = wires_for_hit.at(i);
        // wire_handles is a vector<handle<recob::Wire>>
    }
}
```

#### Consumer side — Options A/B (`ref<T>` in signatures)

```cpp
// Illustrative — Options A/B
void analyze(std::vector<recob::Hit> const& hits,
             phlex::association<recob::Wire, recob::Hit> const& assns,
             phlex::product_store const& store) {
    auto wire_handle = assns.at(0).left().resolve(store);
}
```

---

### 2.4 How Much Explicit Phlex Machinery Users Must Touch

**Option C (framework pre-resolves):** Algorithm author writes a pure function; no
`ref<T>` types, no `resolve()` calls, no `product_store` access. Reference relationships
are declared entirely in registration. Lowest user-facing burden. Cost: registration API
complexity; cannot support algorithms that need to *store* a reference in an output
product, since `handle<T>` is non-persistable (`fwd.hpp:11`).

**Options A/B (`ref<T>` visible in signatures):** Algorithm author must understand `ref<T>`
value semantics, `resolve()` calls, `product_store` access, and how to mint `ref<T>`
values in output products. Higher burden, but enables algorithms to store and forward
references in output products.

**In all cases — new registration machinery:** Regardless of option, the registration API
(`PHLEX_REGISTER_ALGORITHMS`, `m.transform`, `product_selector`, `input_family`,
`output_product_suffixes`, `algorithm.hpp:76-78`) is new machinery with no art
equivalent.

**Current state — no framework-managed validity:** Until the reference mechanism is
implemented, users must eliminate framework references entirely and use intrinsic keys —
`geo::WireID`, `raw::ChannelID_t`, G4 track IDs (`SRTrueParticle.h:33,39-40`), or
integer surrogates (`SRTrack.h:41-42`). These have no framework-managed validity.

**The unresolved critical piece:** The decision from discussion #520 to alter algorithm
signatures to support persistent references (`AGENTS.md:55-57`,
`persistent-references-and-associations.md:201-203`) has been recorded. The normative
specification of what those signatures look like has not been written. Until the stubs in
`data_modeling.rst:4-10` and `role_of_data_product_concepts.rst:5-10` are filled,
neither the user-facing burden nor the registration API can be definitively characterized
for any of the three options.

---

## Q3 — In-Art, Data-Product-Preserving Migration Categories

This section identifies categories of refactoring that can be applied to LArSoft
algorithms while remaining within the art framework, without altering persisted data
products, and while preserving verifiable correctness. The purpose is to reduce the size
and risk of the subsequent Phlex migration step.

---

### Category 1: Hoist Reference Resolution to Module/Cell Boundaries

**Mechanism.** Many art modules construct `FindManyP`/`FindOneP` and call `.at()`
throughout their algorithm body (`AnalysisTree_module.cc:5181-5196`,
`UBXSec_module.cc:547-555`). Hoisting moves all resolution calls to the top of
`produce()`/`analyze()`, before any business logic executes. A local container is
populated from `FindManyP.at(i)` for all `i`, and the business logic function receives
`std::vector<recob::Hit const*> const&` rather than `FindManyP<recob::Hit>&`.

```cpp
// Before: FindManyP threaded through business logic
art::FindManyP<recob::Hit> fmht(trackListHandle, evt, fHitLabel);
for (size_t iTrk = 0; iTrk < nTrks; ++iTrk) {
    auto const& hits = fmht.at(iTrk);
    process_track(hits, ...);
}

// After: resolve at module boundary, pass plain containers
art::FindManyP<recob::Hit> fmht(trackListHandle, evt, fHitLabel);
std::vector<std::vector<recob::Hit const*>> hits_per_track(nTrks);
for (size_t i = 0; i < nTrks; ++i)
    hits_per_track[i] = fmht.at(i);
process_tracks(hits_per_track, ...);
```

**Why it eases Phlex migration.** Business logic becomes a function from plain C++
containers to plain C++ containers — structurally identical to the Phlex pure-function
signature. The `FindManyP` boundary is the only framework-touching point.

**Verifiability.** Bit-exact if the hoist does not change iteration order. A golden-output
harness (analogous to `compare_hits.py`) verifies no output field changes.

**Risks.** `FindManyP` performs lazy evaluation; eager pre-resolution can expose
ordering-dependent bugs previously hidden by deferred access. Memory overhead increases
because all association pointers for an event are resident simultaneously.

---

### Category 2: Isolate Framework-Touching Code Behind Thin Adapters

**Mechanism.** Art module classes mix framework boilerplate (`ServiceHandle`,
`consumes<>`/`produces<>`, `Event::get`/`Event::put`) with business logic. Extract the
business logic into a free function that takes and returns plain C++ types; the module's
`produce()` becomes a thin adapter: get product, call free function, put result. This is
exactly the transformation demonstrated by the gauss\_hit\_finder migration
(`find_hits_with_gaussians.hpp:58-62`), where `void GausHitFinder::produce(art::Event& evt)`
was replaced by
`std::vector<recob::Hit> find_hits_with_gaussians(cfg const&, std::vector<recob::Wire> const&, ...)`.

```cpp
// Adapter (art-only, remains in art codebase)
void MyModule::produce(art::Event& evt) {
    auto const& wires = evt.getByLabel<std::vector<recob::Wire>>(fWireLabel);
    auto hits = find_hits(config_, wires);
    evt.put(std::move(hits));
}

// Core (framework-agnostic, migrates to Phlex)
std::vector<recob::Hit>
find_hits(Config const& cfg, std::vector<recob::Wire> const& wires);
```

**Why it eases Phlex migration.** The free function is the Phlex algorithm. The art
adapter module is deleted and replaced by a Phlex registration wrapper
(`PHLEX_REGISTER_ALGORITHMS`, `algorithm.hpp:76-78`). No logic changes are required in
the core.

**Verifiability.** The free function can be unit-tested without an art event. A
golden-output test confirms the adapter produces identical results to the original
monolithic module.

**Risks.** Art services accessed via `ServiceHandle<T>` inside the original business
logic must be refactored into explicit constructor or function arguments before the free
function is genuinely art-independent.

---

### Category 3: Replace `FindManyP`/`FindOneP` with Pre-Gathered Plain-C++ Inputs

**Mechanism.** Even after Category 1 hoisting, `FindManyP` objects remain at the module
boundary. This category eliminates them from all helper functions by converting them to
pre-gathered containers before any function boundary is crossed. The idiom
`FindManyP<recob::Hit> fmht(...); fmht.at(iTrk)` (`AnalysisTree_module.cc:5181-5196`)
becomes a `std::vector<std::vector<recob::Hit const*>>` populated once at the art module
boundary.

**Why it eases Phlex migration.** The extracted function has no `FindManyP` dependency.
Its input signature — a collection and a parallel vector-of-vectors — maps directly to
the pre-resolved views that a Phlex query helper (e.g., `query::many<T>` under Option C)
would provide.

**Verifiability.** Bit-exact: the data accessed is identical; only the access pattern
changes. Golden-output comparison verifies correctness.

**Risks.** For high-multiplicity events (five `FindManyP` from the same handle as in
`UBXSec_module.cc:547-555`), pre-gathering all associations simultaneously can materially
increase peak memory usage within a single `produce()` call.

---

### Category 4: Replace `art::Ptr` Data Members in Product Types with Intrinsic Domain Keys

**Mechanism.** Product types that carry `art::Ptr` as data members embed a framework
dependency in the persistable schema. Replace with intrinsic domain keys — detector
scalars or geometry identifiers without framework dependency — as performed in the
gauss\_hit\_finder migration (`Hit.h:10-13,52-73`): `art::Ptr<Wire> fWire` and
`art::Ptr<RawDigit> fRawDigit` were removed and replaced with `raw::ChannelID_t fChannel`
and `geo::WireID fWireID`. The duneanaobj CAF types demonstrate the same pattern at
analysis scale: `SRTrack.h:41-42` uses `vector<TrueParticleID>` and
`SRTrueParticle.h:33,39-40` uses integer G4 track IDs in place of framework references.

This change **alters the persisted schema** and must be coordinated with the experiment's
data model versioning and art I/O dictionary evolution.

**Why it eases Phlex migration.** The product type becomes framework-agnostic and can be
used directly in Phlex without a Translator. The Phlex-native reference mechanism (once
designed) can be introduced as a separate association product rather than as an embedded
payload field.

**Verifiability.** The `compare_hits.py` golden-output workflow verifies field content
after the replacement. Fields corresponding to dropped references should be explicitly
excluded from comparison and documented, as done in the gauss\_hit\_finder harness
(`print_hits_to_file.cpp:67-74`, `README.md:220-226`).

**Risks.** Loss of framework-managed reference validity: a `geo::WireID` does not confirm
that the referenced wire exists in the current detector geometry, whereas `art::Ptr`
validity is checkable at access time. Schema evolution must be handled carefully — art
I/O versioning with `schema_version` and ROOT I/O rules is required to keep previously
written data files readable.

---

### Category 5: Introduce Golden-File / Regression Harnesses

**Mechanism.** Before any migration step, establish regression baselines that can verify
each subsequent step end-to-end. The gauss\_hit\_finder example (`compare_hits.py`,
`art_hits_0.txt`–`art_hits_4.txt`, approximately 188,426 lines each) demonstrates the
pattern: add a text-output module (`print_hits_to_file.cpp:26-37`) alongside the
production module; run both in the same art job; commit the text output as golden files;
enforce `compare_hits.py`-style comparison (line count must match; content must be within
a tolerance threshold — the gauss\_hit\_finder uses <3% differing lines to accommodate
threading nondeterminism) in CI.

Fields that depend on unported services must be excluded explicitly and documented
(`print_hits_to_file.cpp:67-74`); these exclusions are technical debt, not accepted
permanent differences.

**Why it eases Phlex migration.** The harness transfers directly: the Phlex algorithm
produces the same text output, and `compare_hits.py` compares against the art golden
files without requiring any Phlex ↔ art data-product interoperability infrastructure.
Each Q3 refactoring step and each Q4 migration step can be verified independently using
the same harness.

**Verifiability.** Golden-output (semantic match within tolerance). Bit-exact comparison
can be added for deterministic algorithms.

**Risks.** Golden files become stale if algorithm physics changes; the CI harness verifies
stability, not correctness. Floating-point output precision and container sort order must
be stable across platforms, compiler versions, and parallelism configurations. The
tolerance threshold must be calibrated per algorithm and documented.

---

### Category 6: Decompose Multi-Output and Conditionally-Producing Modules

**Mechanism.** Art modules that conditionally produce one or two collections depending on
configuration (`README.md:135-138`) or that produce many heterogeneous products in a
single `produce()` call (`TPCObjectMaker_module.cc:378-391`) do not map cleanly to
Phlex's `output_product_suffixes` mechanism (`algorithm.hpp:76-78`). Decompose: introduce
separate helper functions, one per logical output product, so each function maps to one
Phlex transform node.

```cpp
// Before: one produce() with conditional dual output
void HitFinder::produce(art::Event& evt) {
    auto [filtered, all_hits] = find_hits_dual(wires);
    evt.put(std::move(filtered), "filtered");
    if (fSaveAll) evt.put(std::move(all_hits), "all");
}

// After: separate functions per output
std::vector<recob::Hit>
make_filtered_hits(std::vector<recob::Wire> const&, Config const&);

std::vector<recob::Hit>
make_all_hits(std::vector<recob::Wire> const&, Config const&);
```

**Why it eases Phlex migration.** Each Phlex transform node has a clear single output
type and suffix. The `output_product_suffixes("hits")` registration pattern is satisfied
without conditional logic.

**Verifiability.** Each function can be unit-tested independently. Golden-output comparison
for each output product separately.

**Risks.** Shared intermediate computation between the two outputs may need to be
restructured, potentially duplicating work. Implicit ordering dependencies between
multiple `put` calls may surface.

---

## Q4 — Remaining Migration Steps After In-Art Preparation

Once the in-art preparation steps from Q3 are complete — business logic extracted as free
functions, references hoisted to boundaries or replaced by intrinsic keys, golden
harnesses in place — the following steps remain to complete migration into Phlex. Steps
are ordered by dependency. Actionability is assessed against the current state of the
Phlex codebase and the open design decisions enumerated below.

---

### Step 1: Register the Extracted Algorithm Core as a Phlex Operator

**What it involves.** The free function produced by Q3 Category 2 is already structurally
the Phlex algorithm. Write the registration wrapper and the jsonnet/FHiCL configuration
that wires it into a Phlex workflow.

```cpp
// Illustrative — analogous to register_find_hits_with_gaussians.cpp:74-88
PHLEX_REGISTER_ALGORITHMS(m, config) {
    m.transform("find_hits_with_gaussians", &find_hits_with_gaussians, config)
     .concurrency(concurrency::unlimited)
     .input_family(product_selector{type_t<recob::Wire>{}})
     .output_product_suffixes("hits");
}
```

**Phlex machinery involved.** `PHLEX_REGISTER_ALGORITHMS`, `m.transform`,
`product_selector`, `concurrency`, `output_product_suffixes` (`algorithm.hpp:76-78`).

**Actionability.** Actionable today for algorithms whose inputs are whole-product types
with no reference inputs and no service dependencies. This is the only step demonstrated
end-to-end in the existing migration (`register_find_hits_with_gaussians.cpp`).

---

### Step 2: Declare Product Families and Data-Layer Configuration

**What it involves.** Each algorithm must be associated with a data layer and wired to the
appropriate cell scope in the jsonnet/FHiCL configuration. The `product_selector` `layer`
key and the `data_layer_hierarchy` configuration establish which data cells the transform
operates on (`register_find_hits_with_gaussians.cpp:87-88`).

**Phlex machinery involved.** Jsonnet/FHiCL configuration, `data_layer_hierarchy`,
`layer_path`, `product_selector` with layer qualification.

**Actionability.** Actionable today.

---

### Step 3: Port the Persistent-Reference Representation

**What it involves.** For algorithms that consumed or produced `art::Assns` — dropped
entirely in the gauss\_hit\_finder migration (`README.md:142-146`) — the Phlex-native
reference mechanism must be designed and implemented before these algorithms can be fully
migrated. Once a candidate (Q1 Options A, B, or C) is normatively specified:

- Define the algorithm-facing reference type (filling gaps in `data_modeling.rst:4-10`
  and `role_of_data_product_concepts.rst:5-10`)
- Promote `association<A,B,D>` from its current empty placeholder (`types.hpp:5-6`) to a
  real model type in `phlex/phlex/model/`
- Define `element_index` as a concrete type (currently absent from the model layer)
- Implement association construction with a Phlex analog to `art::PtrMaker`/`addSingle`
  (`Assns.h`)
- Extend `product_selector`/`input_family` to declare reference inputs in algorithm
  registration (no mechanism exists; `algorithm.hpp:76-78` handles only whole-product
  inputs)

**Actionability.** **Blocked on OD-1** (choice of algorithm-facing reference type).

---

### Step 4: Implement FORM Persistence of References and Associations

**What it involves.** FORM's `Token` (`token.hpp:12`) provides the I/O-layer address. The
mapping from the Phlex reference triple `⟨data_cell_index, product_specification, element_index?⟩`
(`persistent-references-and-associations.md:311`) to a `Token` does not currently exist,
and `Storage_Write_Association` (`storage_write_association.hpp:12`) is a no-op stub.
This step requires:

- Implementing the reference-triple → `Token` write mapping in `Storage_Write_Association`
- Implementing the `Token` → reference-triple read mapping for reference resolution at
  read time
- Generating or hand-authoring `classes_def.xml`/`LinkDef.h` entries for
  reference-carrying types; if the IDL/code-generator path is adopted (OD-3), ROOT IDL
  becomes a generated artifact
- Extending the HDF5 backend (if applicable) with an analogous association-container
  representation

**Phlex machinery involved.** `phlex/form/`, `Storage_Write_Association`, `Token`,
ROOT and/or HDF5 persistence backends.

**Actionability.** **Blocked on OD-1** (reference type design) and **OD-3**
(IDL/code-generator decision).

---

### Step 5: Wire End-to-End Verification Against Art Golden Outputs

**What it involves.** Use the golden files and `compare_hits.py`-style comparison
established in Q3 Category 5 to verify that Phlex output matches art output across the
full algorithm execution. For algorithms that previously produced associations, the
verification must be extended to cover association content once FORM association
serialization (Step 4) is implemented.

Fields that remain excluded from comparison due to missing Phlex services (e.g., geometry:
`print_hits_to_file.cpp:67-74`, `README.md:220-226`) are technical debt items that must
be tracked explicitly and resolved in Step 6. The exclusion set should shrink
monotonically as services are ported.

**Actionability.** Actionable today for algorithms whose output is fully covered by
existing golden files. Blocked on Step 4 for algorithms with association outputs.

---

### Step 6: Port Geometry and Framework Services

**What it involves.** Algorithms that use `art::ServiceHandle<geo::Geometry>` or other
art services require Phlex equivalents before they can be fully migrated. In the
gauss\_hit\_finder migration, the geometry dependency manifests as `geo::WireID` being
always default-constructed (`find_hits_with_gaussians.cpp:359-365`). Full equivalence
requires:

- Implementing a geometry provider in Phlex (or accepting geometry data as an explicit
  algorithm input)
- Implementing equivalents for other art services used by the algorithm (message logger,
  `TFileService` for diagnostic histograms, calibration services)
- Once services are available, removing field exclusions from the golden-output harness
  and verifying full semantic agreement

The `cell_id` provider pattern (`register_find_hits_with_gaussians_cell_id.cpp:9-15`)
demonstrates that simple scalar providers are already supportable.

**Actionability.** Partially actionable today for simple providers. **Blocked on OD-5**
(framework service availability in Phlex) for geometry and other complex services.

---

## Open Questions for the Design Team

The following decisions are required before the blocked migration steps above can proceed.
They are stated as open questions; this report presents candidate options and trade-offs
but does not prescribe answers.

### OD-1: Choice of algorithm-facing reference type (Q1 Options A, B, C)

The design triple `⟨cell_coord?, product_spec_id, element_index?⟩` is established
(`persistent-references-and-associations.md:311`), but the algorithm-facing C++ type that
exposes it is not. Q1 presents three candidate options. Until one is normatively specified
— filling the stubs in `data_modeling.rst:4-10` and `role_of_data_product_concepts.rst:5-10`
— Steps 3 and 4 cannot be implemented. Discussion #520 decided to support persistent
references and alter algorithm signatures (`AGENTS.md:55-57`); the signature change itself
is not yet designed.

### OD-2: Cross-cell reference resolution mechanism

For inter-cell references (issue #75), `cell_coord` in the reference triple is non-null,
meaning the referenced product resides in a different data cell from the referencing
algorithm's cell. The framework must maintain a registry of product stores across the cell
hierarchy and make it accessible during algorithm execution. The mechanism — whether a
callback, a future/promise, a multi-store argument in algorithm signatures, or a separate
resolution phase — is not yet designed.

### OD-3: IDL/code-generator adoption

Issue #74 anticipates that supporting persistent references may require a domain-specific
language for user-authored data product schemas
(`persistent-references-and-associations.md:207-209`). The discussion draft concludes
that no surveyed third-party IDL provides the typed references with the cardinality,
direction, and cell-scoping semantics required by Phlex, and recommends a Phlex-native
description vocabulary with a code generator that emits ROOT and HDF5 layout artifacts.
This recommendation is non-normative. The decision between adopting a Phlex-native IDL
(making ROOT `classes_def.xml`/`LinkDef.h` generated artifacts) and requiring users to
hand-author those files for reference-carrying types affects Step 4 concretely.

### OD-4: Conditional and multi-output algorithm support

Phlex's current `output_product_suffixes` mechanism does not support conditional output
or dual output from a single transform node (`README.md:135-138`). Q3 Category 6
describes the in-art decomposition that mitigates this, but the Phlex-side design — whether
conditional transforms are expressed in workflow configuration, as separate registered
operators, or through a new registration API — has not been specified. Modules with
configurable output multiplicity cannot be fully migrated until this is resolved.

### OD-5: Framework service availability in Phlex

Art services — geometry (`geo::Geometry`), message logger, `TFileService`, calibration
services — have no Phlex equivalents. Algorithms that depend on these services can be
only partially migrated until Phlex provider equivalents are designed and implemented.
The scope of the Phlex provider API — which services are in scope for the framework to
provide versus which are the experiment toolkit's responsibility — is an open boundary
question between the framework and toolkit layers.

---

## References and Source Anchors

### Phlex design documents (normative)

- `doc/ch_conceptual_design/data_organization.rst:46,62-63` — data products, ROOT/HDF5
  backend requirement
- `doc/conceptual_design.rst:72` — subscript notation `GoodHits₃,₅,₉` as canonical
  persistent-reference coordinate
- `design_agreements.md:113,248` — data-product concept must be persisted; concept can
  have multiple concrete types
- `doc/ch_technical_design/role_of_data_product_concepts.rst:5-10` — pure todo stub
  (normative home for IDL/concept content)
- `doc/ch_technical_design/data_modeling.rst:4-10` — mostly todo stub (normative home
  for data-type definition mechanism)

### Phlex design documents (non-normative / exploratory)

- `persistent-references-and-associations.md:7-8` — status: discussion draft, design
  analysis only
- `persistent-references-and-associations.md:111-112` — "a persistent reference is not a
  serialized handle"
- `persistent-references-and-associations.md:201-203` — discussion #520 decision to
  support persistent references and alter algorithm signatures
- `persistent-references-and-associations.md:207-209` — issue #74, IDL may be required
- `persistent-references-and-associations.md:211-213` — issue #75, inter-data-cell
  references
- `persistent-references-and-associations.md:311` — design triple definition
- `persistent-references-and-associations.md:322-325` — element_index optionality
- `persistent-references-and-associations.md:412-414` — gap: promote association
  placeholder and give Storage_Write_Association real semantics
- `AGENTS.md:55-57` — discussion #520 summary
- `AGENTS.md:69-70` — cell_coord optionality semantics

### Phlex framework C++ sources

- `phlex/phlex/model/data_cell_index.hpp:20` — data_cell_index type
- `phlex/phlex/model/product_specification.hpp:17` — product_specification type
- `phlex/phlex/model/handle.hpp:46` — `handle<T>` type
- `phlex/phlex/model/fwd.hpp:11` — `handle<T>` non-persistable annotation
- `phlex/phlex/model/products.hpp:55,79-85` — type-erased product bag, add_all
- `phlex/phlex/model/product_store.hpp:21,118-122` — product_store, `get_handle<T>`
- `phlex/test/mock-workflow/types.hpp:5-6` — `association<A,B,D>` empty placeholder
- `phlex/test/mock-workflow/largeant.cpp:12-19` — multi-input/output algorithm with
  association output
- `phlex/test/mock-workflow/three_tuple_algorithm.cpp:11-13` — multi-output transform
- `phlex/test/mock-workflow/ion_and_scint.cpp:9-10` — fan-out algorithm
- `phlex/test/mock-workflow/MC_truth_algorithm.cpp:9` — seed/unfold style algorithm
- `phlex/test/mock-workflow/algorithm.hpp:76-78` — registration DSL
- `phlex/form/core/token.hpp:12` — FORM Token (fileName, containerName, technology, id)
- `phlex/form/storage/storage_write_association.hpp:12` — Storage_Write_Association stub

### canvas / art sources (fetched from GitHub)

- `canvas/Persistency/Common/Ptr.h` — `art::Ptr<T>` (ProductID + key + transient cache)
- `canvas/Persistency/Common/PtrVector.h` — compact persistent `vector<Ptr<T>>`
- `canvas/Persistency/Common/Assns.h` — `art::Assns<L,R,D>` persistent storage
- `canvas/Persistency/Common/FindMany.h.in` — FindMany/FindManyP template (generated)
- `canvas/Persistency/Common/FindOne.h.in` — FindOne/FindOneP template (generated)
- `canvas/Persistency/Provenance/ProductID.h` — 32-bit CRC32 of canonical product name
- `art/Framework/Principal/Event.h` — getValidHandle, put, PutHandle
- `art/Framework/Principal/Handle.h` — `Handle<T>`, `ValidHandle<T>`
- `art/Persistency/Common/PtrMaker.h` — `PtrMaker<T>` two-step minting

### LArSoft usage examples (local)

- `ubana/ubana/searchingfornues/ShowerReco/ShowerMerger_module.cc:208-212` — PtrMaker
  construction
- `ubana/ubana/searchingfornues/ShowerReco/ShowerMerger_module.cc:508-509` — mint Ptr
  after emplace_back
- `ubana/ubana/searchingfornues/ShowerReco/ShowerMerger_module.cc:513-527` — addSingle
  pattern
- `ubana/ubana/UBXSec/Modules/RecoTrueMatcher_module.cc:144-194` — util::CreateAssn
- `ubana/ubana/UBXSec/Modules/TPCObjectMaker_module.cc:378-391` — composite object
  linked to multiple Assns
- `ubana/ubana/UBXSec/Modules/TPCObjectMaker_module.cc:525-532` — intra-collection
  PFParticle::Daughters()
- `ubana/ubana/UBXSec/Modules/UBXSec_module.cc:547-555` — five FindManyP from same handle
- `ubana/ubana/UBXSec/Modules/UBXSec_module.cc:641-642,1135-1137` — inter-collection
  cross-module chaining
- `ubana/ubana/Filters/CCInclusiveFilter/NuCCproducer_module.cc:43-46` — three upstream
  producers via FindManyP
- `duneana/duneana/AnaTree/AnalysisTree_module.cc:5181-5196` — `FindManyP<recob::Hit>`
  from track handle
- `duneana/duneana/AnaTree/AnalysisTree_module.cc:4840-4848` — `FindOneP<MVAPIDResult>`
- `duneana/duneana/AnaTree/AnalysisTree_module.cc:4591-4601` — hptr.key() as index for
  chained FindManyP
- `duneanaobj/duneanaobj/StandardRecord/SRTrack.h:41-42` — integer surrogate key
  (TrueParticleID)
- `duneanaobj/duneanaobj/StandardRecord/SRTrueParticle.h:33,39-40` — G4 track ID
  integer references

### Migration example sources

- `phlex-examples/migration/gauss_hit_finder/copied_from_larsoft_minor_edits/Hit.h:10-13,52-73`
  — art::Ptr removal, geo::WireID retention
- `phlex-examples/migration/gauss_hit_finder/find_hits_with_gaussians.hpp:58-62` — Phlex
  algorithm signature
- `phlex-examples/migration/gauss_hit_finder/find_hits_with_gaussians.cpp:98-108,359-365`
  — WireID default-constructed (geometry not available)
- `phlex-examples/migration/gauss_hit_finder/register_find_hits_with_gaussians.cpp:74-88`
  — registration wrapper
- `phlex-examples/migration/gauss_hit_finder/register_find_hits_with_gaussians_cell_id.cpp:9-15`
  — cell_id provider pattern
- `phlex-examples/migration/gauss_hit_finder/print_hits_to_file.cpp:26-37,67-74` —
  golden-output writer, excluded fields
- `phlex-examples/migration/gauss_hit_finder/compare_hits.py` — <3% line-difference
  threshold
- `phlex-examples/migration/gauss_hit_finder/README.md:135-138` — dual output unsolved
- `phlex-examples/migration/gauss_hit_finder/README.md:142-146` — associations dropped
- `phlex-examples/migration/gauss_hit_finder/README.md:220-226` — accepted discrepancies
