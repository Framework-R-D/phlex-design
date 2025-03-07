Introduction
============

[Blurb about definitions and layout of document?]
[Adjustable behavior via plugins.]

Framework scope
---------------

The following diagram illustrates three high-level computing stages commonly used in HEP to obtain physics results from detector signals.

.. image:: framework-scope.png

Many analysis needs can also be met by a data-processing framework.
However, the HEP community tends to perform final-stage analysis using standalone applications.
Phlex, therefore, aims to satisfy the data-processing needs of only physics reconstruction and simulation.

Framework philosophy
--------------------

Requirements process
--------------------

Programming languages
---------------------

The framework will support user algorithms each of which is written in either C++ (no standard yet specified) :dune:`5.1 Support C++ algorithms` or Python (no version yet specified) :dune:`5.2 Support Python algorithms`.
If there is a need to support user algorithms written in another programming language, a corresponding stakeholder requirement should be created.

Note that the language is left unspecified for the implementation of the framework itself.
