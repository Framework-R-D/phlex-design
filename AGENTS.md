# Phlex Design — AI Agent Instructions

Persistent context for AI agents working in the `phlex-design` repository.
Supplements the global `~/.config/kilo/AGENTS.md` and the architecture/build
notes in the sibling `phlex/AGENTS.md`. This repo holds **design and
documentation**, not framework code.

## Repository ecosystem (co-located siblings)

| Path | Role |
| --- | --- |
| `phlex` | Framework C++/Python implementation (has its own detailed `AGENTS.md`) |
| `phlex-design` | This repo: conceptual + subsystem design (Sphinx/RST, Quarto, Markdown) |
| `phlex-examples` | Example user programs, incl. a LArSoft hit-finder migration |
| `phlex-coding-guidelines` | Coding standards |
| `../art-devel/canvas` | `art` framework reference implementation (`Ptr`, `Assns`, `ProductID`) |

## Phlex in one paragraph

Phlex (**P**arallel, **h**ierarchical, **l**ayered **ex**ecution) is a
functional, graph-based data-processing framework for HEP (primarily DUNE),
the successor to `art`/Meld. Algorithms are pure operators to higher-order
functions (transform, fold, unfold); the framework owns scheduling (TBB) and
all data-product lifetime. Phlex is **only the framework**: it provides tools
and protocols for experiments and toolkits (LArSoft, NuTools, GENIE, Geant4) to
describe *their own* data models — it does not define data product types itself
(`doc/ch_conceptual_design/data_organization.rst:62-63`).

## Core data model (vocabulary — used everywhere)

- **Data product**: smallest unit of user data with framework-tracked
  provenance; not contained in other products (only referenced via "views").
  Type defined by the user, not the framework.
- **Data cell**: a grouping of data products identifiable by the framework,
  tagged with a **data layer** label and a unique index. `art` analog: a `Run`,
  `SubRun`, or `Event` object.
- **Data layer / data-layer hierarchy**: experiment-defined aggregation levels;
  top is always `Job`. Arbitrary depth, e.g. `Job → Run → Spill → APA`. Replaces
  `art`'s fixed `Run`-`SubRun`-`Event`.
- **Indexed family**: collection indexed by an (unordered) index set; Phlex
  processes families, not ordered sequences. `data-cell family` /
  `data-product family` arise from `unfold`.
- **Data-product concept vs concrete type**: one concept may have many concrete
  representations (languages/layouts); a **Translator** converts between
  concretes modeling the same concept.
- **handle<T>**: in-memory, non-persistable view of a product (raw pointers).
- Subscript notation `GoodHits₃,₅,₉` = product in APA 9 / Spill 5 / Run 3
  (`doc/conceptual_design.rst:66`); this *is* the persistent-reference coordinate.

## Design status (as of 2026-06)

- Conceptual design is mature; **subsystem design has many `.. todo::` stubs**,
  notably `ch_subsystem_design/role_of_data_product_concepts.rst` and
  `ch_subsystem_design/data_modeling.rst` (the IDL / data-modeling homes).
- There is **no normative IDL/DDL yet**. Issue #74 anticipates needing one;
  discussion #520 (July 2025 retreat) decided to support persistent references
  à la `art::Ptr` and alter algorithm signatures accordingly.
- Standalone discussion docs live in the repo root (`dune-fw-design-ideas-chg.md`,
  `dune-req-009-issues.md`, this file's companion
  `persistent-references-and-associations.md`).

## Persistent references & associations (researched 2026-06)

Full analysis: `persistent-references-and-associations.md` (this repo).
Key facts to avoid re-deriving:

- A **persistent reference** is the triple
  `⟨ cell_coord?, product_spec_id, element_index? ⟩`, NOT a serialized `handle`.
  `cell_coord` optional (defaults to referrer's cell; present ⇒ inter-cell ref
  per phlex issue #75). `element_index` optional (present ⇒ member of a
  collection product, the `art::Ptr::key` role).
- Identity substructure already exists in `phlex/phlex/model/`:
  `data_cell_index` (which cell), `product_specification` (creator
  plugin+algorithm + suffix + type_id), `handle` (in-memory only).
- **Associations are data products**: `phlex/test/mock-workflow/types.hpp` has a
  placeholder `template<typename A,typename B,typename D=void> struct
  association{};`; `largeant.cpp:12-19` returns one in the output tuple. Direct
  analog of `art::Assns<A,B,D>` (a `vector<pair<Ptr<L>,Ptr<R>>>` + optional `D`;
  `FindOne`/`FindMany` are transient query views).
- **FORM backend** (`phlex/form/`, only place ROOT IDL is allowed) already has
  `Token` = `(file, container, technology, id)` persistent address,
  `Storage_Write_Association` container stub, and confines ROOT
  `classes_def.xml`/`LinkDef.h` to `phlex/test/form/data_products/`
  (with `extra_member/` exercising schema evolution).
- **Present user workaround** (no ref mechanism yet): embed intrinsic domain keys
  instead of framework refs — see migrated LArSoft
  `phlex-examples/migration/gauss_hit_finder/.../Hit.h:9-14` (removed
  `art::Ptr<Wire>`/`art::Ptr<RawDigit>`, kept `geo::WireID`).
- **IDL conclusion**: no surveyed third-party IDL (Protobuf, Cap'n Proto;
  considered also FlatBuffers/Thrift/Avro/ASN.1/SBE/JSON Schema) supplies typed
  references/associations with cardinality+direction (C3) or cell scoping (C4).
  Recommended path = Option A: a small Phlex-native description vocabulary +
  code generator that emits FORM ROOT IDL and HDF5 layouts (ROOT IDL becomes a
  generated artifact, never hand-authored by users).

## Building the docs

Sphinx project under `doc/` (`doc/Makefile`, `conf.py`); Quarto `.qmd` files and
`_quarto.yml` at root; `GNUmakefile` drives top-level builds. `rstcheck` config
in `.rstcheck.cfg`. Respect markdownlint rules (MD012/MD022/MD031/MD032/MD034/
MD040) and the global file-formatting rules (one trailing newline, no trailing
whitespace).
