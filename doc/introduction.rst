************
Introduction
************

A resonable definition of a software framework is [Wiki-Framework]_:

  *an abstraction in which software, providing generic functionality, can be selectively changed by additional user-written code, thus providing application-specific software.*

The framework orchestrates data flow, resource management, and parallel execution.
It enables a scientific collaboration to write standardized workflows where physicists can insert their own algorithms.
In a HEP context, this insertion often occurs by the framework dynamically loading libraries called *plugins*.
Although not required, a framework often provides a program's `main(...)` function, which (directly or indirectly) invokes user code within the plugins as configured at appropriate points in the program's execution.

The following diagram illustrates three high-level computing stages commonly used in HEP to obtain physics results from detector signals.

.. graphviz:: framework-scope.gv

Frameworks are typically used in a high-level trigger environment, for reconstructing physics objects from detector signals, or for simulating physics processes.
Many analysis needs can also be met by a data-processing framework.
However, the HEP community tends to perform final-stage analysis using standalone applications.
Phlex, therefore, aims to satisfy the data-processing needs of only physics reconstruction and simulation.

============================================
Requirements Process and Framework Selection
============================================

Phlex provides facilities and behaviors that support the physics goals of its stakeholders, notably the DUNE experiment [#f1]_.
In a concerted effort in 2023 and 2024, DUNE established a set of high-level requirements or *stakeholder requirements*, which constrain the design of a framework in support of DUNE's needs.
A dedicated tool [Jama-Connect]_ was subsequently selected to manage such stakeholder requirements, tracking them in a version-controlled manner, and creating logical dependencies among them.
Additionally, *system requirements* were created to guide implementation in support of the stakeholder requirements.

After formulating its stakeholder requirements, DUNE evaluated whether existing HEP frameworks could also satisfy DUNE's needs.
The frameworks considered included the *Gaudi* framework [Gaudi]_ (used by ATLAS and LHCb), *CMSSW*'s framework [CMSSW]_, ALICE's *O2* framework [O2]_, as well as the *art* framework [art]_, which is used by many of the intensity-frontier experiments at Fermi National Accelerator Laboratory.
Over time, each framework has undergone substantial adjustments to take advantage of hardware and software developments (multi-threading, multi-processing, GPU usage, etc.), resulting in more efficient data-processing, and (in some cases) making possible some data-processing that may have been infeasible without adjustment.

Each framework considered above, however, is designed according to event-centric, collider-physics concepts.
The DUNE stakeholder requirements demand that any event-centric assumptions must be relaxed to permit more flexible data organizations (see :numref:`introduction:Flexibility`).
Specifically, the data groupings of interest must not be rigidly defined by the framework itself, but specifiable by the user :need:`DUNE 22`.
Even *art*, the framework used by the ProtoDUNE experiments, forces users to adopt awkward workarounds to process the relatively slow-evolving detector signatures of neutrino interactions (as compared to the very fast beam interactions of collider experiments).

To be sure, an existing HEP framework can be modified to eventually meet the needs of DUNE.
However, such modification does not exist in a vacuum, and in DUNE's determination, adjusting an existing HEP framework to satisfy DUNE's stakeholder requirements was impractical.
Significant changes would be required for any of the frameworks, and it is unclear the manner and extent to which such changes would be accepted by already operational experiments using the framework in question.
DUNE thus decided to develop a new framework designed to directly support DUNE's framework requirements.
Establishing a new framework also provides an opportunity to shed legacy coding patterns that have been problematic in achieving efficient data-processing.

----------------------
Requirements Ownership
----------------------

Each Phlex stakeholder owns its stakeholder requirements, which support the high-level needs of the experiment.
System requirements, which are subservient to stakeholder requirements, are owned by the Phlex developers, who are free to adjust the implementation to satisfy all stakeholder requirements.

-----------------------------
Requirements in This Document
-----------------------------

The stakeholder requirements are listed in :numref:`Appendix %s <appendices/requirements:Framework requirements>` for convenience.
To more easily connect the design to the requirements, any design aspect influenced by specific requirements contains bracketed references to those requirements (e.g. :need:`DUNE 22`).

Where possible, we limit references to stakeholder requirements to the conceptual design in :numref:`Chapter %s <conceptual_design:Conceptual design>`.
Some stakeholder requirements are referenced in :numref:`Chapter %s <supporting_design:technical design>` if those requirements do not affect the conceptual framework model.
No system requirements are currently referenced in this document.

====================
Framework Philosophy
====================

A framework is a tool that aids the scientific process of inferring accurate physics results from observed data.
Maintaining data integrity is therefore paramount, as is retaining an accounting of how physics results were obtained from that data.
The Phlex design therefore:

- treats all data presented to (or created by) Phlex as immutable for the remainder of a Phlex program's execution,
- requires recording the :term:`provenance` of every created :term:`data product` :need:`DUNE 121`, and
- enables, and---to the extent possible---ensures the :term:`reproducible` creation of data products.

-----------
Flexibility
-----------

Physics results in HEP are obtained by processing large collections of data and making statistical statements from them.
Each element of a collection generally contains the data corresponding to one readout of the detector.
Such elements are often termed "events", which are treated as statistically independent observations of physics processes.
It is common for experiments to define larger aggregations of data by grouping events into subruns (or, for LHC experiments, luminosity blocks), and by further grouping subruns into runs.
These larger aggregations are typically defined according to when certain detector calibrations or accelerator beam parameters were applied.

Although frameworks supporting the `Run`\ -\ `Subrun`\ -\ `Event` (RSE) hierarchy have proved effective and flexible enough for collider-based experiments, the RSE hierarchy is not always appropriate:

- simulated data often do not need to be processed with an RSE hierarchy; a flat hierarchy (e.g. only the "event") is usually sufficient,
- framework interface is often explicitly couched in RSE terminology, making it difficult to apply to non-collider contexts, where a different data-grouping may be more appropriate (e.g. time slices for extended readout windows, each of which correspond to one "event"),
- calibration data is often described independently from an RSE hierarchy, requiring other means of accounting for systematic corrections that must be applied to the data.

Phlex does not prescribe an RSE hierarchy—it only requires that the hierarchy be representable as a directed acyclic graph (DAG) at run-time, with each grouping of data represented as a node in the graph, and the relationships between data-groupings represented as edges.
This expression of the hierarchy greatly relaxes the constraints placed on experiments while still supporting the collider-based RSE hierarchy (see :numref:`ch_conceptual_design/data_organization:data layers, data cells, and families`).

The hierarchy graph and its nodes (i.e. the data-groupings) are definable at run-time, thus allowing the specification of data organizations that are appropriate for the workflow :need:`DUNE 22`.

The flexibility in defining data-groupings and how they relate to each other necessitates further flexibility:

1. user-defined algorithms are not bound to statically-typed classes representing data-groupings—e.g. there is no direct dependency on a C++ "event" class, and
2. a framework program must be "driven" by a user-provided entity that expresses the hierarchy graph desired by the user, not a hierarchy that is prescribed by the framework.

These concepts are discussed more fully in :numref:`Chapter %s <conceptual_design:Conceptual design>`.

-----------
Portability
-----------

Phlex is intended to be used on a variety of computing systems to take advantage of the disparate computing resources available to each stakeholder :need:`DUNE 8`.
This means the framework:

- must support data-processing by algorithms that execute on GPUs :need:`DUNE 11`, in addition to those that execute on CPUs,
- may not generally rely on hardware characteristics unique to a particular platform :need:`DUNE 63`,
- must favor standardized programming-language features.

---------
Usability
---------

Although usability is not a formal stakeholder requirement, physicists expect various behaviors and features that ease one's interaction with a data-processing framework.
Phlex strives to meet this expectation in various ways:

    *minimizing boilerplate code*
        Some data-processing frameworks in HEP adopt an object-oriented design, where stateful framework-dependent objects are required to register inherently framework-agnostic algorithms with a framework program.
        Phlex does not generally require physics algorithms to depend on any framework libraries :need:`DUNE 43`.
        This design, therefore, substantially reduces the amount of code required for the interface between physics algorithms and the framework itself (see :numref:`introduction:Framework Independence`).

    *failing early*
        To avoid needless computation, Phlex will fail as early as possible in the presence of an error.
        This means that, for C++ usage, compile-time failures will be favored over run-time exceptions.

    *meaningful error messages*
        When failures within the scope of the framework occur [#errors]_, the reported error messages will be as descriptive as possible.
        Messages will typically include diagnostic information about the data being processed when the error occurred as well as the algorithms that were executed on that data.

    *graceful shutdown*
        For run-time errors, the default behavior of Phlex is to end the framework program gracefully :need:`DUNE 134`.
        A graceful shutdown refers to a framework program that completes the processing of all in-flight data, safely closes all open input and output files, cleans up connections to external entities (such as databases), etc. before the program ends.
        This ensures that no resources are left in ill-defined states and that all output files are readable and valid.

-----------
Reusability
-----------

The decision to create a new framework is appropriately met with skepticism.
However, the selection of which framework design to pursue was strongly guided by past R\&D efforts specifically targeted to explore DUNE's framework needs [Meld]_.
In addition, many ideas already represented in existing production-quality frameworks are also integrated into Phlex's technical design (*under preparation*):

- Existing parallel-processing libraries will be used (e.g. Intel's oneTBB),
- ROOT IO mechanisms will be in place for persisting data,
- The HEP-CCE experience on portability technologies will inform how CPU/GPU source-code portability is achieved.

The chance to develop a new framework also necessitates a re-examination of the knowledge acquired by the broader computing community, and how that knowledge can be applied to data-processing in HEP.
Consequently, part of the Phlex design is simply a summary of prior research that has a ready application to DUNE's data-processing needs (e.g. see :numref:`Chapter %s <preliminaries:High-Level Abstractions>`).

=====================
Programming Languages
=====================

The framework will support user :term:`algorithms <algorithm>` written in multiple programming languages :need:`DUNE 14`.  Specifically, an algorithm may be written in either C++ [#f2]_ :need:`DUNE 81` or Python [#f3]_ :need:`DUNE 82`.
If there is a need to support user algorithms written in another programming language, a corresponding stakeholder requirement should be created.

Note that the language is left unspecified for the implementation of the framework itself.

======================
Framework Independence
======================

We define an algorithm as *framework-independent* if it contains no explicit dependencies on framework libraries—i.e. it is possible to build and execute the algorithm independent of a framework context.
For framework-independent C++ algorithms, this means there are no direct or transitive framework libraries that are either included as headers in the algorithm code or linked as run-time libraries.
Similarly, framework-independent Python algorithms import no direct or transitive framework packages.

Phlex is required to support the registration of user-defined, framework-independent algorithms :need:`DUNE 43`.
This does not mean that *all* framework-independent algorithms are suitable for registration, nor does it mean that all algorithms registered with the framework must be framework-independent.
In fact, depending on what the algorithm is doing, some algorithms might require explicit framework dependencies.

==============================
Guide to Reading This Document
==============================

This document is intended to convey Phlex's conceptual design without delving into implementation specifics.
We therefore adopt a set of high-level abstractions that model the data-processing the framework will perform.
These ideas are discussed in :numref:`Chapter %s <preliminaries:High-Level Abstractions>`, which summarizes well-known mathematical concepts already used in the computing industry.
From there, we discuss the conceptual design itself in :numref:`Chapter %s <conceptual_design:Conceptual design>`.

.. only:: conceptual_design_only

   Appendices are provided that give definitions (:numref:`Appendix %s <appendices/glossary:Glossary>`) and list stakeholder requirements (:numref:`Appendix %s <appendices/requirements:Framework Requirements>`).

.. only:: not conceptual_design_only

   Supporting material is presented in :numref:`Chapter %s<supporting_design:technical design>`.

   Appendices are provided that give definitions (:numref:`Appendix %s <appendices/glossary:Glossary>`), list stakeholder requirements (:numref:`Appendix %s <appendices/requirements:Framework Requirements>`), present supported platforms (:numref:`Appendix %s <appendices/platforms:Supported Platforms>`), and discuss Phlex deployment (:numref:`Appendix %s <appendices/deployment:Deployment>`).

.. attention::

   Any C++ or Python framework interface presented in this document is illustrative and not intended to reflect the final framework interface.

.. rubric:: Footnotes

.. [#f1] It is possible for additional experiments to become stakeholders of the Phlex framework.
         In such a case, the stakeholder requirements of one experiment may not negate those of another (particularly DUNE).
.. [#errors] Any errors that occur within an algorithm must be handled by the algorithm itself, unless the intention of the algorithm author is to allow the error to propagate up to the code that invokes the algorithm.
.. [#f2] As of this writing, Phlex supports the C++23 standard.
.. [#f3] Phlex adheres to SPEC 0 [SPEC-0]_ in supporting Python versions and core package dependencies.

.. only:: html

   .. rubric:: References

.. [Wiki-Framework] https://en.wikipedia.org/w/index.php?title=Software_framework&oldid=1285034658
.. [Jama-Connect] https://www.jamasoftware.com/platform/jama-connect/
.. [Gaudi] Charles Leggett, *et al*, J. Phys. Conf. Ser. **898**, 042009 (2017)
.. [CMSSW] E. Sexton-Kennedy, *et al*, J. Phys. Conf. Ser. **608**, 012034 (2015)
.. [O2] J. Adam, *et al* [ALICE Collaboration], “Technical Design Report for the Upgrade of the Online-Offline Computing System”, CERN-LHCC-2015-006, ALICE-TDR-019 (2015)
.. [art] C. Green, *et al*, J. Phys. Conf. Ser. **396**, 022020 (2012)
.. [Meld] K. Knoepfel, EPJ Web of Conferences **295**, 05014 (2024)
.. [SPEC-0] https://scientific-python.org/specs/spec-0000/
