---
type: analysis
project: persistent-references
date: 2026-11-06
date_captured: 2026-11-06
status: completed
tags:
  - Phlex
  - analysis
summary: Sourced answers on pre-write validation, shared target identity, endpoint metadata, multi-target resolution, and PODIO's Apache-2.0 licence; implemented capabilities are distinguished from proposed extensions.
related:
  - "[[2026-11-06 draft review questions prompt]]"
  - "[[status-2026-10-30]]"
---

# Persistent-reference draft review — 6 November 2026

## Scope and evidence boundary

This report answers the [[2026-11-06 draft review questions prompt|five draft-review questions]]. The [[status-2026-10-30|October 30 project checkpoint]] records the earlier state; its uncommitted-branch status is historical, not the state verified for this review.

The review covered the working-copy draft and README, applicable technical-design material, and validation branch `persistent-references-option-c` at commit `219e0b48b2137844c8c993993df0f1dae576be32`. That branch is now committed; the documents' "uncommitted" statements are stale. Current framework HEAD `fa056e5e66feaef70d928a03d96c0578b680e02c` was also checked for existing metadata interfaces. The requested branch was read through versioned Git operations without switching the checkout. The available semantic index covered only the `persistent-references` directory, so technical-design and sibling-framework evidence was checked through scoped file reads and versioned Git reads.

Throughout this report, **implemented**, **intended design**, and **proposed extension** are distinguished. The persistent-reference draft remains non-normative. Tests were inspected, not rerun; this investigation does not supply new persistence or execution validation. Two read-only research subtasks investigated metadata/multi-target scheduling and authoritative licensing sources; their principal findings were cross-checked before synthesis. No question was skipped for model-capability reasons.

Primary local documents:

- [`README.md` — documentation map and decision ledger](file:///Users/greenc/work/cet-is/sources/phlex/phlex-design/persistent-references/README.md).
- [`persistent-references-explorations-2026-09-30-result.md` — current proposed design and survey](file:///Users/greenc/work/cet-is/sources/phlex/phlex-design/persistent-references/persistent-references-explorations-2026-09-30-result.md).
- [`metadata_management.rst` — intended handle-accessible metadata](file:///Users/greenc/work/cet-is/sources/phlex/phlex-design/doc/ch_technical_design/metadata_management.rst).
- [`glossary.rst` — authoritative terminology](file:///Users/greenc/work/cet-is/sources/phlex/phlex-design/doc/appendices/glossary.rst).

Unless otherwise specified, framework code citations below refer to the validation commit above in `/Users/greenc/work/cet-is/sources/phlex/phlex`, not necessarily the currently checked-out files. Design-document citations refer to the inspected working copies; the draft and README are untracked, so the design repository's HEAD alone does not identify their contents.

## 1. Can references be verified before FORM writes them?

**Yes, but the current prototype does not provide comprehensive pre-write verification.**

### What happens currently

- **Local output references** receive output-slot, type, and element-bounds checks after the algorithm returns and before its products are published.
- **Already-bound references** are left untouched by that binding step. `fully_bound()` checks only that no local references remain—not that their targets exist or their indices are valid.
- Even `ref_to(handle, index)` does **not** check the index when minting. The test at `test/reference_resolution_test.cpp:220–225` deliberately mints an out-of-range reference and detects it only during resolution.
- A resolving consumer checks target identity, type and bounds before its algorithm runs. This could happen in the same job; it need not wait until a later read. But an association written without first passing through such a consumer can escape those checks.
- The inspected FORM writer has no integration for validating Phlex association endpoints. Association persistence itself remains the unprototyped Stage 1 item.

Evidence: validation-branch `phlex/model/reference_token.hpp:124–150`, `phlex/model/association.hpp:73–97`, `phlex/core/transform_node.hpp:97–103`, and `phlex/core/reference_resolution.hpp:193–279`; draft §§4.1, 8 and 10.

### A suitable extension

Introduce a **framework-controlled reference-validation step**, covering *all* association endpoints after local binding. It would validate against the producing invocation's inputs and returned outputs, or another explicitly supported target scope.

For persistence, require a validation gate **before handing the association to FORM's write operation**. An unrelated validation observer is insufficient unless an explicit dependency prevents the writer from running first. Likewise, validation at `commit_place()` would be too late to guarantee "before the backend writes": `register_write()` already calls `fill_container()` at `form/persistence/persistence_writer.cpp:195`.

The validation need not construct a resolved association and its navigation hash maps. Checking target identity, compatible representation and collection length is sufficient for referential integrity.

Two limits matter:

1. **Unavailable targets cannot simply be declared valid.** Strict verification needs their data or sufficiently trustworthy target descriptions, with appropriate readiness and retention.
2. **Referential integrity is not physics correctness.** An incorrect relationship that names an existing, in-range element of an allowed product will pass structural validation.

There should also be an explicit output policy: must targets be written with the association, remain accessible in an input dataset, or be allowed as external references? Validity in memory today does not by itself ensure resolvability from the eventual output.

The draft's raw-token forwarding escape hatch is an API choice, not a logical necessity for forwarding. An opaque, typed forwarding value could reduce accidental fabrication, but mandatory validation is still the substantive safeguard.

## 2. Must every pair carry both cell coordinates?

**No. Full logical identity does not require repeating its physical representation in every row.**

There is a terminology correction: the current resolver requires **one target product instance per side per invocation**, which also determines one cell coordinate per side. It is not merely a one-cell restriction.

The association data structure itself does not enforce that restriction. Its rows can contain tokens naming different products and cells; the current resolver rejects those outside its two selected targets.

For an association whose endpoints are genuinely uniform, a representation could store:

- one absolute left-target description;
- one absolute right-target description;
- per-row element indices and optional `D`.

That would factor out repeated product locators as well as repeated cell coordinates.

This does **not** contradict the mandatory-coordinate decision, README D1. Every reference still has an unambiguous absolute identity; the representation shares common information rather than omitting it. Forwarding can reconstruct the full token when needed.

A general target table, discussed under question 4, would support both uniform and multi-target associations. Packing must first validate the identities: it must not silently replace a differing row coordinate with the common header coordinate.

Thus duplication in the prototype is **not a required storage contract**. Whether factoring is worthwhile should be assessed during the FORM persistence spike; the proposed storage layout has not yet been demonstrated.

Evidence: draft §5.1; README D1 and D11; validation-branch `phlex/model/association.hpp:43–61` and `phlex/core/reference_resolution.hpp:253–279`.

## 3. Can an algorithm interrogate metadata of an association target?

**For target identity, yes today. For general provenance or arbitrary attached metadata, the intended design is broader than the implemented interface.**

First, `D` is a **pair-specific payload stored within the association**, not a separately managed framework product. Product-level metadata should normally be accessed through the target rather than duplicated in every relationship payload.

### Currently available

A resolved row exposes `left_token` and `right_token`. These identify:

- the target cell coordinate;
- creator;
- product suffix;
- stage name;
- optional element index.

Consequently, an algorithm can inspect this identity directly without using `D`.

An ordinary target input supplied as `handle<Product>` additionally exposes `data_cell_index()`, `creator()`, `suffix()`, `layer()`, `layer_path()` and `stage()`. These handle interfaces are identical on the checked current HEAD and requested branch.

However, the resolved endpoints are **pointers, not target handles**. Neither the tokens nor current handles provide a general target-provenance or arbitrary-metadata accessor. Current handles also lack the public data-product-concept accessor envisaged by the technical design.

Evidence: validation-branch `phlex/core/resolved_association.hpp:35–41`; `phlex/model/handle.hpp:88–98`, also verified at current HEAD; technical design `doc/ch_technical_design/metadata_management.rst:18–31`.

### Appropriate interface direction

A useful extension would give each resolved endpoint access to a **read-only target metadata view**, populated by the framework during resolution. The resolver already knows the target store, product specification and stage.

For an element reference, this must identify the **containing product** and its metadata—not misrepresent the element as a separately managed product. Target metadata could be shared through the resolved association's target table.

The algorithm would need either:

- this framework-provided metadata view; or
- plain metadata values extracted by a registration wrapper.

It does **not** need an event object, product store or global lookup registry. The wrapper approach is already illustrated by the conceptual design's handle-based registration adapter at `doc/ch_conceptual_design/registration.rst:190–216`.

General provenance—parentage, configuration and versions—and typed user metadata require further metadata-management implementation. A stage name is one identity/provenance coordinate, not a complete provenance record; the distinction is discussed in `stage-name-analysis.md:5–11,77–87`. Borrowed metadata views would need the same invocation-lifetime contract as handles and resolved endpoint pointers.

## 4. Can multiple targets be supported without enumerating every target in resolve or requiring a global store?

**Yes as an architectural direction, but metadata alone is insufficient, and the current resolver does not implement it.**

"D3" is interpreted here as **draft §3 D3, consumer target declaration**, which is **README D5**. README D3 instead concerns minting. "Multiple targets" is interpreted as multiple product instances or cells with compatible endpoint types—not arbitrary different C++ types on one side.

### Representation

Existing tokens already contain the identities needed to distinguish multiple targets. A proposed compact layout could be:

```text
targets[id] = {absolute cell coordinate, product locator, product description}
row         = {left target id, left index?, right target id, right index?, D}
```

The product description would provide concept/schema and representation information where required. The table is an identity/description mechanism; it does not itself make a target available. Cross-job identity and provenance would still need the later-stage contract recorded in the draft, rather than being inferred from the Stage 0 locator.

### Scheduling and resolution

The consumer could select the association, optionally applying endpoint or target filters. The **potential target scope** could instead come from producer declarations, an association description, or persisted reference-dependency metadata.

The framework would then use a logical **grouped target input** and a **resolver-private, scoped lookup**:

1. Identify eligible product families and the cell domain.
2. Collect or load the needed targets.
3. Index them by full cell coordinate **and product locator**—a cell can contain several producer stores and products.
4. Establish readiness and retain the targets.
5. Validate and resolve the required endpoints before dispatch.
6. Release retained products when their consumers finish.

This eliminates one **user-visible port declaration per target**, not the underlying dependencies. Those dependencies can be represented or expanded internally.

The difficulty depends on the target domain:

| Target domain | Required mechanism |
| --- | --- |
| Finite product families known at graph initialization | Internal dependency expansion/grouped inputs |
| Runtime-created cells in known families under a bounded ancestor | Gather, completion accounting and retention |
| Exact targets discovered only from association rows | A discovery phase, plus protection against targets being released before discovery |
| Arbitrary targets outside a bounded domain | Broader dynamic loading/scheduling, cycle, completion and eviction policies |

A process-wide global store is **not logically required**. A resolver-private scoped index is still a lookup structure, but it need not be global or accessible to algorithms.

Accordingly, the draft's statements that runtime deduction necessarily requires a global registry, and that three ports are intrinsic, are too strong as general architectural conclusions. They describe the prototype's strategy. **Readiness and retention are intrinsic; that particular port encoding is not.**

Current FORM product dictionaries and navigation tables locate persisted products; they are not association dependency manifests. Current input configuration also does not automatically discover association dependencies from those dictionaries. That functionality would require additional work.

A gather must distinguish **all cells announced** from **all required products ready**. Missing or filtered targets, zero-cell domains, multiple stores per cell, late arrivals and bounded memory need explicit policies. The existing ancestor repeater does not provide descendant/sibling gathering.

Evidence: draft §§3, 5.2–5.3 and 8; technical design `doc/ch_conceptual_design/data_organization.rst:73–97`; current-HEAD `phlex/core/producer_catalog.cpp:85–137`, `phlex/core/index_router.cpp:398–447`, `phlex/core/detail/repeater_node.cpp:87–115,163–195`, `form/form_source.cpp:90–149,177–205` and `form/persistence/persistence_writer.cpp:461–565`.

### art and DUNE

Local canvas confirms that one `art::Assns<L,R,D>` can contain **different ProductIDs on the same side**, with fixed endpoint types. `addSingle` imposes no common-ProductID restriction, and `IPRHelper.h:176–183` explicitly discusses associations referring to multiple source collections.

Evidence in `/Users/greenc/work/cet-is/sources/art-devel/canvas`: `canvas/Persistency/Common/Assns.h:130–132,208–210,359–365,463–483` and `canvas/Persistency/Common/detail/IPRHelper.h:176–183`.

That supports the relevance of the capability. However, the experimental examples checked—forwarding references and consuming numerous associations—do not establish that a particular execution uses multiple ProductIDs per side, nor that this capability is an adopted DUNE requirement.

The appropriate next requirement example is concrete: one association linking compatible elements from **two distinct target products**, followed separately by a cross-cell example. This would test whether grouped/scoped resolution meets the needed semantics rather than assuming the current three-selector contract is sufficient.

## 5. Does PODIO's GPL licence prevent these projects or Phlex from using it?

**The draft's central premise is incorrect: current PODIO is Apache-2.0, including the cited `v01-09` release.**

This is supported by primary project information, not inferred from downstream usage:

- [PODIO `v01-09` license, pinned to its release commit](https://raw.githubusercontent.com/AIDASoft/podio/0b9021bb695800c3e86ff536a2b4bd062932ae3c/LICENSE):

  > "Licensed under the Apache License, Version 2.0"

- [Current PODIO license, commit-pinned](https://raw.githubusercontent.com/AIDASoft/podio/fc7cb38c6abfaadd933a68d9947692fd9faed0ad/LICENSE) declares the same terms.
- [PODIO issue #324](https://github.com/AIDASoft/podio/issues/324) records the proposed GPL-to-Apache change and contributor sign-offs.
- [Commit `1f8f560`, 6 December 2022](https://github.com/AIDASoft/podio/commit/1f8f560619053e5ba5ad44f128e7aa77ca123837) is titled "move to Apache 2.0 license as discussed in #324."
- [EDM4hep](https://raw.githubusercontent.com/key4hep/EDM4hep/main/LICENSE) and [k4FWCore](https://raw.githubusercontent.com/key4hep/k4FWCore/main/LICENSE) declare Apache-2.0 individually. This does not imply one licence for the entire Key4hep ecosystem.

The projects genuinely use PODIO for generation **and runtime linkage**. EDM4hep's [model build](https://raw.githubusercontent.com/key4hep/EDM4hep/main/edm4hep/CMakeLists.txt) invokes PODIO generation macros; PODIO's [build macros](https://raw.githubusercontent.com/AIDASoft/podio/fc7cb38c6abfaadd933a68d9947692fd9faed0ad/cmake/podioMacros.cmake) link generated model libraries to `podio::podio`. [k4FWCore's build](https://raw.githubusercontent.com/key4hep/k4FWCore/main/k4FWCore/CMakeLists.txt) links PODIO I/O and EDM4hep.

Inspection of current generator files, templates and representative runtime files found no separate GPL notice. The resolution is **Apache relicensing**, not a discovered GPL linking or generated-code exception. This was not an exhaustive historical exception or dependency audit.

### Consequence for Phlex

The [Apache-2.0 terms](https://www.apache.org/licenses/LICENSE-2.0.txt) permit use and preparation of derivative works. They support evaluating reuse or adaptation of current PODIO rather than assuming a clean-room replacement is legally necessary.

Distribution obligations still apply: licence copies, relevant attribution, retained notices and change notices. Generated code can contain template-derived implementation, schema material and injected `ExtraCode`; absence of a generated licence banner does not make it public domain. Third-party dependencies retain their own terms.

Even under the counterfactual GPL premise, differing repository licences alone would not establish infringement: Apache-2.0 material can be included in a GPLv3 combination, although that does not authorize relicensing GPL-derived material as Apache-only. See [ASF compatibility guidance](https://www.apache.org/licenses/GPL-compatibility.html).

For older GPL snapshots, running a GPL generator does not automatically impose GPL on all output; copied generator/template text has to be assessed separately. A generated-code exception would not automatically exempt a separately linked runtime. Private use also differs from distribution. These distinctions are explained in the FSF's [GPL-tools/output guidance](https://www.gnu.org/licenses/gpl-faq.html#CanIUseGPLToolsForNF), [linking guidance](https://www.gnu.org/licenses/gpl-faq.html#GPLStaticVsDynamic) and [private-use guidance](https://www.gnu.org/licenses/gpl-faq.html#GPLRequireSourcePostedPublic). They are not needed to resolve the current PODIO licence premise.

**Recommended correction:** change Appendix A's PODIO licence to **Apache-2.0** and remove §9.2's licence-based clean-room rationale. Architectural suitability, hierarchical references, backends and maintenance costs remain separate technical questions.

This is published-license analysis, not blanket legal clearance for a particular Fermilab distribution. Such clearance would require the actual versions, modified/generated files, dependency inventory and distribution arrangement. Relicensing authority for every historical contributor was not independently audited. The examined evidence supplies no basis for alleging infringement by EDM4hep or k4FWCore.

### Public source snapshots

The following public HEADs were verified on 6 November 2026. Current-branch links above were inspected at these snapshots; commit-pinned license links are supplied for the central PODIO finding.

| Repository | Branch | Commit |
| --- | --- | --- |
| PODIO | `master` | `fc7cb38c6abfaadd933a68d9947692fd9faed0ad` |
| EDM4hep | `main` | `2ed05afc7e79f8f688959b43056bbe02109a96e2` |
| k4FWCore | `main` | `f95187d5cd69d1e079e170e4e2e7f6d828e6a92d` |

## Recommended follow-up boundaries

1. Specify comprehensive association validation and the output-target persistence policy before implementing a pre-write gate.
2. Evaluate shared target descriptions during the Stage 1 FORM round-trip experiment rather than treating the prototype's per-row duplication as a storage requirement.
3. Separate target identity access, general provenance access and typed user metadata in the algorithm-facing contract.
4. Validate multi-product and multi-cell requirements with concrete DUNE use cases; design grouped/scoped readiness and retention before extending resolution.
5. Correct the PODIO licensing premise before deciding whether generator reuse or adaptation is technically worthwhile.

These are recommendations for separately authorized design or implementation work. This review did not modify the framework or design documents, adopt a new requirement, or establish persistence readiness.

## Capture record

The user explicitly authorized direct writes to Brian's directory tree after the initial research report identified the unavailable project-document capture interface. `obsidian vault=Brian vault info=path` resolved the vault to `/Users/greenc/Library/CloudStorage/OneDrive-FermiNationalAcceleratorLaboratory/Documents/Brian`. The existing `10_Projects/Phlex/persistent-references` directory and related notes were checked before creating this report. The earlier research and its evidence dates remain 6 November 2026; the capture does not imply additional framework tests or legal review.
