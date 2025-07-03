===============================
Topology of the data-flow graph
===============================

The graph shown in :numref:`workflow` is a close reflection of the graph used for scheduling algorithm execution.  It is formed from multiple ingredients, the most crucial of which is the program configuration (see :numref:`ch_conceptual_design/program_configuration:Program configuration`).
It is the program configuration that specifies:

- the driver to use,
- the algorithms needed for creating, filtering, and observing data products,
- the specification of data products required and/or created by each algorithm,
- which data products to write to output files,
- which data products to read from input files,
- any resources needed to process the data, and
- any other constraints to impose on the program.

The pattern used for constructing the data-flow graph is described in detail in :numref:`ch_subsystem_design/task_management:Creating the data-flow graph`.
Here it is sufficient to note that the graph is primarily driven by which data products are specified to be written to output (i.e. *precious* data products).
By default, the framework will schedule for execution only those algorithms needed to write precious data products.
This means that not necessarily every algorithm in the program configuration will be used in a general workflow.

Each input file is able to report which data products it contains, and each algorithm-registeration statement (see :numref:`ch_conceptual_design/registration:Framework Registration`) declares which data products are required :need:`DUNE 65` and which data products are created :need:`DUNE 156` by the algorithm.
These data-product dependencies, along with the dependencies implied by the resources, are sufficient to establish the data-flow graph.
