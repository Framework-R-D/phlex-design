Building the data-flow graph
============================

Phlex adopts the data-flow approach discussed in the :ref:`section on data flow with sequences <data_flow:Data flow with sequences>`.
Instead of expressing scientific workflows as monolithic functions to be executed, workflows are factorized into composable algorithms :dune:`1 Algorithm Decomposability` that operate on data products :dune:`1.1 Algorithm Communication Via Data Products` passed among them :dune:`9 Presenting data to subsequent algorithms`.
These algorithms then serve as operators to higher-order functions that operate on *data-product sequences*.


The framework is a system of integrated aspects that must work together.
To illustrate how these subsystems are designed to work together, we use the below graph to describe each aspect.

.. todo::
   Make graph
