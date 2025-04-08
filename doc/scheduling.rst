Building the data-flow graph
============================

Phlex adopts the data-flow approach discussed in the :numref:`section on data flow with sequences (%s) <data_flow:Data flow with sequences>`.
Instead of expressing scientific workflows as monolithic functions to be executed, workflows are factorized into composable algorithms that operate on data products passed among them :dune:`1 Algorithm Decomposability,1.1 Algorithm Communication Via Data Products,9 Presenting data to subsequent algorithms`.
These algorithms then serve as operators to higher-order functions that operate on *data-product sequences*.


The framework is a system of integrated aspects that must work together.
To illustrate how these subsystems are designed to work together, we use the below graph to describe each aspect.

.. todo::
   Make graph
