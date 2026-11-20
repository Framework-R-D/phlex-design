# Persistent References – Option C Analysis (De novo)

**Scope** – This document provides a fresh, independent analysis of **Option C** (“Opaque token resolved by the framework before algorithm dispatch”) as described in the [prompt](persistent-references-explorations-2026-09-17_02-prompt.md). It is written from the *algorithm author*’s point‑of‑view, evaluates the design trade‑offs, identifies the DSL and schema‑definition requirements, and presents concrete recommendations for proceeding.

---

## 1.  High‑level description of Option C

* **Opaque token** – a lightweight, serialisable identifier (`phlex_ref` triple) that is not passed as an algorithm input. The framework resolves input tokens *before* `execute()` and supplies **`handle<T>`** objects directly; output-token creation is a separate contract below.

* **Algorithm signature** – resolved inputs are **`handle<T>`** values or views. Algorithms do not receive stored reference tokens or access `product_store` / call `resolve()`; an algorithm may explicitly mint an opaque output token through the output API below.

* **Framework responsibilities**
  1. **Reference‑input declaration** – during registration the author specifies every reference relationship (e.g. `input_association<Wire, Hit>("wire‑hit‑assn")`).
  2. **Cross‑cell resolution** – for references that cross data‑cell boundaries the framework must look‑up the target `product_store` using the `cell_coord` part of the token.
  3. **Output reference minting** – when an algorithm needs to *output* a reference, it may call `mint_ref<T>(target_handle, element_index)`, which returns an opaque `phlex_ref` value for the output product. FORM serializes that token; this does not expose stored tokens as algorithm inputs.

---

## 2.  Algorithm‑author point‑of‑view

| Concern | How Option C addresses it | Open question / mitigation |
|---|---|---|
| **Pure‑function mental model** | Author writes a normal C++ function taking containers and `handle<T>` inputs. No framework objects appear in the body. | None – matches existing `three_tuple_algorithm.cpp` style. |
| **Reference inputs** | Declared once in registration DSL; author receives a resolved association view with both endpoint handles and optional link payload. | Need a concise way to request *only the required subset* of an association (e.g. per‑track). May require a **filter DSL** on the resolved view. |
| **Reference outputs** | Author explicitly calls `framework::mint_ref<T>(target, idx)` to create an opaque `phlex_ref` output value; token fields remain hidden. | Define how the framework binds or resolves reference-bearing product fields; this embedded-field path is not part of the initial association-input contract. |
| **Cross‑cell references** | Resolved by the framework, author never touches cell identifiers. | The author may need to know the *logical* cell name for debugging; provide optional `debug_cell()` on the handle. |
| **Performance** | Framework can batch‑resolve all declared associations before dispatch, enabling a single pass over the underlying storage. | Must guarantee deterministic ordering for reproducibility; document the ordering guarantees of the resolved-association view. |
| **Error handling** | `handle<T>` is always valid (or throws at registration time if the reference cannot be resolved). | Define a clear compile‑time/registration‑time validation step that fails fast when a reference is missing. |

---

## 3.  Required DSL extensions

### 3.1 Registration DSL (C++‑fluent API)

```cpp
PHLEX_REGISTER_ALGORITHMS(m, cfg) {
    m.transform("find_hits", &find_hits, cfg)
        .concurrency(concurrency::unlimited)
        // input containers
        .input_family(product_selector{type_t<recob::Wire>{}})
        // reference inputs – *Option C* specific
        .input_association<recob::Wire, recob::Hit>("wire‑hit‑assn")
        // optional per‑association filters (future DSL)
        // .where([](auto const& wire){ return wire.id() < 1000; })
        .output_product_suffixes("hits", "wire‑hit‑assn");
}
```

*`input_association`* is the new keyword that expands the existing `input_family` DSL. It captures the proposed **triple** (`cell_coord?`, `product_spec_id`, `element_index?`) for each association. This September proposal was later refined: the current vocabulary uses `element_token{cell_coordinate, product_locator{creator, suffix, stage}, element?}`; the coordinate is mandatory, and product concepts/concrete types are not additional locator fields. The Stage 1 mapping to FORM `product_identity` remains subject to persistence validation.

### 3.2 Output‑reference DSL (runtime API)

```cpp
// Inside algorithm implementation
phlex::ref_token make_hit_ref(
    phlex::handle<recob::Hit> const& hit,
    std::size_t elem_idx)
{
    return framework::mint_ref(hit, elem_idx); // returns opaque token
}
```

The function `mint_ref` is deliberately thin: it validates that `elem_idx` is within range, creates the opaque `phlex_ref` output value, and registers the token for later FORM serialization. It does not provide an algorithm-side conversion from stored tokens to handles.

### 3.3 Schema/IDL DSL for products containing references

Because references are stored as opaque tokens, product schemas must describe **reference fields**.  The design team prefers a **YAML‑based DSL** (human‑readable, easy to version) that can be fed to a code generator.

```yaml
# Example: Hit.yaml (product schema)
name: recob::Hit
fields:
  - name: charge
    type: float
  - name: wire_ref
    type: phlex_ref
    target: recob::Wire
    cardinality: one   # one‑to‑many is expressed via a separate association product
  - name: raw_digit_ref
    type: phlex_ref
    target: raw::RawDigit
    cardinality: one
```

The **code generator** reads the YAML, emits:

* ROOT `classes_def.xml` / `LinkDef.h`
* A framework-owned resolved view or adapter for reference fields, if embedded references are adopted. Its resolution context and generated accessor contract are deferred; algorithms do not convert stored `phlex_ref` values to handles themselves.
* FORM `Token` conversion utilities, with product locators mapped to FORM `product_identity`; the persistence layout still requires round-trip validation

---

## 4.  Trade‑offs compared with Options A/B

| Dimension | Option C (opaque token) | Option A (value‑type `ref<T>`) | Option B (split `product_ref` / `element_ref`) |
|---|---|---|---|
| **Author burden** | Low for inputs: the framework supplies resolved handles/views; output references use the explicit `mint_ref` helper. | Moderate – author must manipulate `ref<T>` and call `resolve()`. | Higher – two distinct reference types to learn. |
| **Ability to store references in outputs** | Requires `mint_ref` helper; adds a small API surface. | Native – `ref<T>` can be stored directly. | Native – `element_ref<T>` can be stored directly. |
| **Cross‑cell handling** | Centralised in framework, transparent to author. | Author must pass `product_store` or registry manually. | Same as A. |
| **Registration‑DSL complexity** | New keywords (`input_association`, `output_association`) needed. | No new DSL – existing `input_family` suffices. | Same as A, plus type‑split handling. |
| **Runtime performance** | Framework can batch‑resolve, potentially lower overhead. | Per‑reference resolve call incurs overhead. | Same as A. |
| **Extensibility (future association topologies)** | Straightforward – new association types are declared in registration only. | Requires exposing more generic `ref<T>` APIs. | Same as A. |

**Conclusion** – Option C offers the best author ergonomics and enables heavy optimisation in the framework, at the cost of a modest increase in registration‑DSL complexity and the need for a tiny runtime API (`mint_ref`).  For the Phlex philosophy of “pure‑function algorithms”, this aligns best with the design goals.

---

## 5.  Implementation roadmap

| Milestone | Description | Owner | Estimated effort |
|---|---|---|---|
| **5.1** | Extend registration DSL: add `input_association` / `output_association` methods, update `algorithm.hpp` and corresponding parser. | Framework team | 2 weeks |
| **5.2** | Cross‑cell registry: design a `cell_registry` that maps `(cell_coord, product_spec_id)` to `product_store*`. Integrate it into the dispatch pipeline. | Runtime‑engine team | 3 weeks |
| **5.3** | `mint_ref` API: implement token creation, validation, and FORM write hook. | Storage team | 1 week |
| **5.4** | YAML schema DSL + code generator: define schema format (see §3.3), add generator to `phlex/tools/idl_generator`. | Tooling team | 4 weeks |
| **5.5** | FORM `Token` ↔ `phlex_ref` mapping: flesh out `Storage_Write_Association` and read path. | FORM team | 2 weeks |
| **5.6** | Unit-test suite: mock `product_store`; verify registration validation, resolved handles, and `mint_ref` round-trip through FORM. | QA team | 2 weeks |
| **5.7** | Documentation & examples: write developer-facing docs and add an algorithm example using an input association and output reference. | Documentation team | 1 week |

Milestones **5.4–5.6** depend on the OD‑3 decision about introducing an IDL/schema DSL; they are not independent of it. The other milestones can proceed in parallel with the existing migration steps (Steps 1–2 from the main migration plan).

---

## 6.  Open decisions (OD‑style) specific to Option C

1. **OD‑C‑1 – Registration‑DSL syntax** – Should the DSL remain fluent (method chaining) or move to a declarative JSON/Jsonnet block?  Recommendation: keep fluent for C++ compile‑time safety; provide a Jsonnet translation layer for generated workflows.
2. **OD‑C‑2 – Filter expression language for associations** – Do we embed a tiny expression DSL (e.g. `where: "wire.id < 1000"`) or leave filtering to the algorithm?  Recommendation: start without filters; add later if performance profiling shows unnecessary data transfer.
3. **OD‑C‑3 – Token visibility** – Should the opaque token be exposed as a type (`phlex_ref`) in product headers, or hide it behind a `using` alias? Recommendation: expose a thin `phlex_ref` struct with private members; resolve stored tokens to handles only through the framework, not through algorithm-side conversion.

---

## 7.  UML sketch (PlantUML)

```plantuml
@startuml
class Algorithm {
    +execute(wires, hits, wire_hits: resolved_association<Wire,Hit,D>)
}

class Framework {
    +register(Algorithm, RegistrationSpec)
    +resolveAssociations()
    +mint_ref<T>(handle<T>, index) : phlex_ref
}

class phlex_ref {
    -cell_coord?: DataCellIndex
    -product_spec: ProductSpecification
    -element_index?: size_t
}

Algorithm --> Framework : registration DSL
Framework --> phlex_ref : creates
phlex_ref --> Token : serialises (FORM)
@enduml
```

*The diagram illustrates the input/output distinction: the framework resolves input references and hands `handle<T>` objects to the algorithm; the algorithm may separately produce opaque output tokens for later serialization.*

---

## 8.  Recommendations

* **Adopt Option C** as the primary reference strategy for Phlex.  It best matches the pure‑function design ethos and enables framework‑wide optimisations.
* **Implement the DSL extensions** in the next sprint (Milestones 5.1‑5.3).  These are low‑risk and give immediate benefit to any algorithm that already declares reference inputs.
* **Choose a YAML‑based schema DSL** (Section 3.3) for product definitions that contain references.  YAML offers readability and can be validated against a JSON‑Schema, satisfying the team’s slight preference for YAML.
* **Defer the filter DSL** until profiling demonstrates a need; keep the registration API simple.
* **Proceed with a small prototype**: a toy algorithm that declares an input association and emits an output reference using `mint_ref`.  Run the full FORM round‑trip test to validate the `Token` mapping.

---

*Prepared by the Kilo analysis agent on 2026‑09‑17.  The document is intended for internal design discussion and as a basis for the upcoming implementation sprint.*
