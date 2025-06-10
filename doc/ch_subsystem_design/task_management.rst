Task Management
===============

.. todo::
   - Includes building the task graph
   - Includes scheduling of tasks as well as any relevant task priorities or concurrency constraints
   - Respecting data-marshaling demands
   - Relates to algorithms and data (product) sets

The execution of computational tasks will be managed by `oneTBB Flow Graph <https://oneapi-spec.uxlfoundation.org/specifications/oneapi/v1.3-rev-1/elements/onetbb/source/flow_graph>`_.
With oneTBB, the user does not explicitly manage threads but specifies tasks that should be scheduled by oneTBB for execution using its own thread pool.

Depending on the need, oneTBB interface may be explicitly used in algorithm code to take advantage of intra-algorithm parallelism.
Such use will integrate smoothly with Phlex's scheduled tasks as both the framework and the algorithm will be using oneTBB's shared thread pool :need:`DUNE 46`.

Creating the Data-Flow Graph
----------------------------

The data-flow graph is constructed using the pattern described in the subsections below.

Program Configuration
^^^^^^^^^^^^^^^^^^^^^

The Phlex program ingests a program configuration that includes a specification of all algorithms that are graph-node *candidates*.

Required Data Products
^^^^^^^^^^^^^^^^^^^^^^

All configured observers and data sinks are constructed as graph nodes.
The framework records the list of data products required by each of these nodes.
A data sink can be configured to accept all data products created by the algorithms in the configuration, without specifying the individual data products created by each algorithm.

Created Data Products
^^^^^^^^^^^^^^^^^^^^^

The transforms, folds, and unfolds specified in the configuration are created as graph nodes.
The framework records the list of the data products produced by each of these nodes.

Connecting Algorithms
^^^^^^^^^^^^^^^^^^^^^

The data product lists recorded in :numref:`ch_subsystem_design/task_management:Required data products` and :numref:`ch_subsystem_design/task_management:Created data products` are compared.
Edges are created between the nodes in :numref:`ch_subsystem_design/task_management:Created data products` that create data products and those that match what is required by those in :numref:`ch_subsystem_design/task_management:Required data products`.
This step is iterative so that all graph-node candidates from :numref:`ch_subsystem_design/task_management:Program configuration`.
