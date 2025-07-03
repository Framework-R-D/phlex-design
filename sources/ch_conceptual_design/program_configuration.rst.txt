Program Configuration
=====================

As described in :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`, a Phlex program starts by ingesting a configuration, which informs how the data-flow graph (e.g. see :numref:`workflow`) should be established.

To support reproducibility of data, a Phlex program is configured by *only one* configuration document :need:`DUNE 69`, although that document may be compiled from multiple distinct sources :need:`DUNE 133` (such as included or imported files).
These documents may be written in the following languages: Jsonnet, YAML, JSON (as a strict subset of Jsonnet), or FHiCL :need:`DUNE 72`.
A given configuration can be written in any of these languages, but no configuration document may be written in more than one language.

The other details and rules by which a Phlex program is configured largely do not inform the conceptual design and are described in :numref:`ch_subsystem_design/configuration:Configuration`.
