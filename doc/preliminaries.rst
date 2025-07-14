***********************
High-Level Abstractions
***********************

With Phlex, the general notion of data-processing must be expressed without relying on the concept of an *event* :need:`DUNE 22`.
We therefore avoid the "event" terminology altogether.
Instead, we adopt a set of well-known abstractions that can be systematically applied to each of the processing behaviors required of Phlex.
These ideas work together, and they are presented here as ingredients necessary for understanding the conceptual design given in :numref:`Chapter %s <conceptual_design:Conceptual design>`.

This chapter is somewhat formal in mathematics.
However, the formality used provides crisp descriptions of the data-processing concepts required of Phlex, enabling a computing model that is (a) naturally thread-safe, and (b) allows a close connection between user-defined algorithms and the physics such algorithms are intended to represent.

As will be discussed in :numref:`ch_preliminaries/functional_programming:Functional Programming`, Phlex adopts a *functional programming* paradigm for the construction of workflows.
To adequately describe what functional programming is, we first introduce function notation, the concept of the *type*, and mathematical sequences and indexed families.
With these ingredients, we are then able to discuss functional programming and how it is supported by data-flow graphs.

.. toctree::
   :maxdepth: 2

   ch_preliminaries/function_notation
   ch_preliminaries/types
   ch_preliminaries/sequences
   ch_preliminaries/functional_programming
   ch_preliminaries/data_flow
