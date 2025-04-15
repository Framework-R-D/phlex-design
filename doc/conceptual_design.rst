*****************
Conceptual design
*****************

.. admonition:: Purpose

   The conceptual design is not a reference manual; it is a high-level description of how the framework aims to satisfy the stakeholder requirements (see :numref:`Appendix %s <requirements:Framework requirements>`).
   The audience for the conceptual design is the physicist, algorithm author, or framework program runner.
   A more technical description of the intended design is given in :numref:`Chapter %s <technical_design:Technical design>`.

Phlex adopts the data-flow approach discussed in :numref:`data_flow:Data flow with sequences`.
Instead of expressing scientific workflows as monolithic functions to be executed, workflows are factorized into composable algorithms that operate on data products passed among them :dune:`1 Algorithm Decomposability,1.1 Algorithm Communication Via Data Products,9 Presenting data to subsequent algorithms`.
These algorithms then serve as operators to higher-order functions that operate on *data-product sequences*.

To guide the discussion of Phlex's conceptual model, we refer to the graph in  :numref:`workflow`, which illustrates various framework aspects:

- the creation of the data-flow graph itself (see :numref:`scheduling:Building the data-flow graph`)
- data products and data-product sets as passed along graph edges (see :numref:`data_organization:Data organization`)
- user-provided algorithms such as transforms, folds, etc. (see :numref:`algorithms:Algorithms` and :numref:`algorithms:HOF operators` )
- the framework driver (see :numref:`sources_sinks:Framework driver`)
- data sources (see :numref:`sources_sinks:Data sources`)
- data sinks (see :numref:`sources_sinks:Data sinks`)
- resources (see :numref:`resources:Resources`)
- program configuration (see :numref:`user_configuration:Program configuration`)

.. graphviz:: graphviz/work-flow.gv
   :caption: Sample workflow that unfolds a data product.
   :name: workflow

.. toctree::
   :maxdepth: 2

   scheduling
   data_organization
   algorithms
   sources_sinks
   resources
   user_configuration
