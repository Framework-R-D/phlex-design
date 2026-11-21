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

.. _translator-node-execution:

Translators
-----------

A translator node consumes one concrete data product and produces one result.
An explicitly registered translator uses an input selector and an output suffix, and participates in the ordinary producer and consumer catalogs without consulting conversion records or checking concept membership.
Its result shares the input's data-cell index but is advertised with the translator as creator, the registered output suffix, and the translator's execution stage.

The design calls for the framework to insert translator nodes automatically when an available product and a required input have different concrete types that model the same concept.
Translator nodes belong to the computational graph; they do not precede providers or follow preservers.
The framework should share one node for the same required translation and use metadata generated at plugin build time to discover converters without loading every translator library.
One conversion record per ordered pair does not by itself ensure that a node is shared for a particular input product.

Proposed ADR 0002 separates registering a conversion function from creating a node and separates the node's execution identity from the identity of the product it advertises.
The catalog representation, discovery format, and details of placement and node sharing are follow-up work.


Drivers and Runtime Hierarchy Handling
--------------------------------------

Phlex will implement a driver interface that enables opening an input file.

The hierarchy of data-cell indices will be declared to the framework before processing any data-cell indices.

Different policies for supporting hierarchies will be supported:

   - a fixed hierarchy may be declared for the entire job
   - the global hierarchy may be updated based on hierarchies stored in input files in the middle of the job

The data-layer hierarchy will not be known until the input file is opened.
Whenever the global hierarchy is established (e.g. by the first input file) or updated (e.g. by a subsequent input file), the framework validates it against what the computational graph requires.
