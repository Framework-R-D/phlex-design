===============================
Topology of the data-flow graph
===============================

The pattern used for constructing the data-flow graph is described in detail in :numref:`subsystems/task_management:Creating the data-flow graph`.
Here we discuss the overall topology of the workflow, using the graph in :numref:`workflow`.

The graph-node candidates are separated into various classifications:

    *creators*
      algorithms that create data (e.g *transforms*, *folds*, and *unfolds*)

    *predicates*
      algorithms that return a Boolean value upon each invocation; each predicate serves as the operator to a filter operation

    *observers*
      algorithms that accept data products as input arguments but do not create new data products
