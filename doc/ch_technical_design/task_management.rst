Task Management
===============

.. todo::
   - Includes scheduling of tasks as well as any relevant task priorities or concurrency constraints
   - Respecting data-marshaling demands
   - Relates to algorithms and data (product) sets
   - Is this where the grammar goes for the predicate expression (:numref:`ch_conceptual_design/hofs/filters:Predicate Expression`)?

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

The data product lists recorded in :numref:`ch_technical_design/task_management:Required data products` and :numref:`ch_technical_design/task_management:Created data products` are compared.
Edges are created between the nodes in :numref:`ch_technical_design/task_management:Created data products` that create data products and those that match what is required by those in :numref:`ch_technical_design/task_management:Required data products`.
This step is iterative so that all graph-node candidates from :numref:`ch_technical_design/task_management:Program configuration`.

Data-Marshaling
---------------

Translators
-----------

A *translator* is a node whose input is of one concrete data-product type and whose output is of a different concrete data-product type, both of which model the same data-product concept.
A translator will be automatically inserted into the graph between two nodes whose data-product types are concept-compatible but not identical.
Translator nodes belong to the computational graph.
There are no translators that precede a provider.
There are no translators that follow a preserver.

From the flow-graph point of view, translators are treated as first-class nodes, while being mostly irrelevant or invisible to the user.
Translator nodes for a given data representation conversion should be singular by construction and should not be duplicated.
Translators are not configured directly by the framework user; they must be discovered by the framework based on needs implied by the user configuration.
The framework should be able to discover translators and select which ones belong in the graph without loading every discovered translator DLL.
The translator metadata required for graph building should be generated at translator plugin build time.


Drivers and Runtime Hierarchy Handling
--------------------------------------

Phlex will implement a driver interface that enables opening an input file.

The hierarchy of data-cell indices will be declared to the framework before processing any data-cell indices.

Different policies for supporting hierarchies will be supported:

   - a fixed hierarchy may be declared for the entire job
   - the global hierarchy may be updated based on hierarchies stored in input files in the middle of the job

The data-layer hierarchy will not be known until the input file is opened.
Whenever the global hierarchy is established (e.g. by the first input file) or updated (e.g. by a subsequent input file), the framework validates it against what the computational graph requires.
Validation may involve creating more index-set nodes, provider nodes, preserver nodes, and translator nodes.
The job shall gracefully end when one of the input files contains a hierarchy that does not meet the hierarchy constraints of the computational graph.

The semantic distinction between framework drivers and data-product providers will be retained.

For release v0.3.0, a *data-cell index iterator* that supplies one data-cell index at a time is sufficient.
This is a temporary solution that will be modified in the future.
Range-based processing of multiple data cells at one time is not needed for v0.3.0.

After v0.3.0, expected future needs include:

   - updating processed hierarchy when opening another input file
   - a mode where external data management declares file hierarchy in advance
   - support for multiple concurrently open input files for multithreading or overlay use cases


