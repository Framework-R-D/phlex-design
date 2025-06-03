************
Introduction
************

The following diagram illustrates three high-level computing stages commonly used in HEP to obtain physics results from detector signals.

.. graphviz:: graphviz/framework-scope.gv

Wikipedia decently defines a software framework as [Wiki-framework]_:

  *an abstraction in which software, providing generic functionality, can be selectively changed by additional user-written code, thus providing application-specific software.*

The framework orchestrates data flow, resource management, and parallel execution.
It enables a scientific collaboration to write standardized workflows where physicists can insert their own algorithms.
In a HEP context, this insertion often occurs by the framework dynamically loading libraries called *plugins*.
Although not required, a framework often provides a program's `main(...)` function, which (directly or indirectly) invokes user code within the plugins as configured at appropriate points in the program's execution.

Frameworks are typically used in a high-level trigger environment, for reconstructing physics objects from detector signals, or for simulating physics processes.
Many analysis needs can also be met by a data-processing framework.
However, the HEP community tends to perform final-stage analysis using standalone applications.
Phlex, therefore, aims to satisfy the data-processing needs of only physics reconstruction and simulation.

====================
Requirements Process
====================

Phlex provides facilities and behaviors intended to support the physics goals of its stakeholders, notably the DUNE experiment [#f1]_.
DUNE has established a set of high-level requirements or *stakeholder requirements*, which constrain the design of the framework in support of DUNE's needs.
A dedicated tool [Jama-connect]_ is used to manage such stakeholder requirements, tracking them in a version-controlled manner, and creating logical dependencies among them.
As the design matures, *system requirements* are then created to guide implementation in support of the stakeholder requirements.

----------------------
Requirements Ownership
----------------------

Each Phlex stakeholder owns its stakeholder requirements, which support the high-level needs of the experiment.
System requirements, which are subservient to stakeholder requirements, are owned by the Phlex developers, who are free to adjust the implementation to satisfy all stakeholder requirements.

-----------------------------
Requirements in This Document
-----------------------------

The stakeholder requirements are listed in :numref:`Appendix %s <requirements:Framework requirements>` for convenience.
To more easily connect the design to the requirements, any design aspect influenced by specific requirements contains bracketed references to those requirements (e.g. :dune:`1 Algorithm Decomposability`).

Where possible, we limit references to stakeholder requirements to the conceptual design in :numref:`conceptual_design:Conceptual design`.
Some stakeholder requirements are referenced in the technical design in :numref:`technical_design:Technical design` if those requirements do not affect the conceptual framework model.
No system requirements are currently referenced in this document.
Should it become necessary to refer to the system requirements, such references will be limited to the technical design in :numref:`technical_design:Technical design`.

====================
Framework Philosophy
====================

A framework is a tool that aids the scientific process of inferring accurate physics results from observed data.
Maintaining data integrity is therefore paramount, as is retaining an accounting of how physics results were obtained from that data.
The Phlex design therefore:

- treats all data presented to (or created by) Phlex as immutable for the remainder of a Phlex program's execution,
- requires recording the :term:`provenance` of every created :term:`data product` :dune:`51 Provenance discovery`, and
- enables, and---to the extent possible---ensures the :term:`reproducible` creation of data products.

=====================
Programming Languages
=====================

The framework will support user :term:`algorithms <algorithm>` written in multiple programming languages :dune:`5 Support for multiple programming languages`.  Specifically, an algorithm may be written in either C++ [#f2]_ :dune:`5.1 Support C++ algorithms` or Python [#f3]_ :dune:`5.2 Support Python algorithms`.
If there is a need to support user algorithms written in another programming language, a corresponding stakeholder requirement should be created.

Note that the language is left unspecified for the implementation of the framework itself.

======================
Framework Independence
======================

We define a algorithm as *framework-independent* if it contains no explicit dependencies on framework libraries—i.e. it is possible to build and execute the algorithm independent of a framework context.
For framework-independent C++ algorithms, this means there are no direct or transitive framework libraries that are either included as headers in the algorithm code or linked as run-time libraries.
Similarly, framework-independent Python algorithms import no direct or transitive framework packages.

Phlex is required to support the registration of user-defined, framework-independent algorithms :dune:`20 Algorithms independent of framework interface`.
This does not mean that *all* framework-independent algorithms are suitable for registration, nor does it mean that all algorithms registered with the framework must be framework-independent.
In fact, depending on what the algorithm is doing, some algorithms might require explicit framework dependencies.

.. rubric:: Footnotes

.. [#f1] It is possible for additional experiments to become stakeholders of the Phlex framework.
         In such a case, the stakeholder requirements of one experiment may not negate those of another (particularly DUNE).
.. [#f2] As of this writing, Phlex supports the C++23 standard.
.. [#f3] Phlex adheres to SPEC 0 [SPEC-0]_ in supporting Python versions and core package dependencies.

.. only:: html

   .. rubric:: References

.. [Wiki-framework] https://en.wikipedia.org/wiki/Software_framework
.. [Jama-connect] https://www.jamasoftware.com/platform/jama-connect/
.. [SPEC-0] https://scientific-python.org/specs/spec-0000/
