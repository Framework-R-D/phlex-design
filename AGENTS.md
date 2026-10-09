# Phlex Design

This repository holds shared design documentation and OpenSpec plans for Phlex.
Framework implementation and user examples belong in their respective code
repositories.

## Read By Task

- **Terminology**: read [GLOSSARY.md](GLOSSARY.md) for shared domain language.
  When changing a definition, consult the
  [glossary](doc/appendices/glossary.rst) and keep corresponding entries aligned.
- **Design**: start at [doc/index.rst](doc/index.rst) and follow its conceptual
  or technical design chapter to the relevant section before changing a design
  claim.
- **Decisions**: use [design_agreements.md](design_agreements.md) to locate
  decision-bearing issues and discussions. Check the linked primary source and
  retain its agreement strength and caveats; the ledger is a dated snapshot.
- **Wiki**: before adding or promoting design notes, read
  [design_wiki/design-ideas-wiki.md](design_wiki/design-ideas-wiki.md) for the
  seeds-to-saplings-to-design lifecycle.
- **Persistence**: for persistent references, associations, or IDL/DDL choices,
  read [persistent-references-and-associations.md](persistent-references-and-associations.md)
  as design research, with proposals and recommendations distinguished from
  agreements.
- **OpenSpec**: before planning, applying, or archiving work, read
  [openspec/config.yaml](openspec/config.yaml) for cross-repository contracts,
  repository locations, and required target-repository guidance. Read the
  relevant change's proposal, specifications, design, and tasks for its scope
  and recorded progress.
- **Sphinx**: before editing or building the design document, read
  [doc/AGENTS.md](doc/AGENTS.md) for local guidance and
  [README.md](README.md) for build setup. Use the build configuration for
  current commands and dependencies.

## Evidence

Identify the source and status of each existing-behavior claim you change.
Describe proposed behavior as proposed; verify current implementation claims
against the target repository's code and tests. OpenSpec task checkboxes record
progress, not proof of implemented behavior. When sources conflict, report the
conflicting paths and scopes before presenting either claim as settled.
