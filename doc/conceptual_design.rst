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

- the data-flow graph itself (see :numref:`scheduling:Topology of the data-flow graph`)
- data products and data-product sets as passed along graph edges (see :numref:`data_organization:Data organization`)
- user-provided algorithms such as transforms, folds, etc. (see :numref:`algorithms:Algorithms` and :numref:`algorithms:HOF operators` )
- the framework driver (see :numref:`sources_sinks:Framework driver`)
- data sources (see :numref:`sources_sinks:Data sources`), which are plugins that provide data products from external entities to downstream user algorithms (e.g. input from ROOT files)
- data sinks (see :numref:`sources_sinks:Data sinks`), which are plugins that may write data products to an external entity (e.g. output files)
- resources (see :numref:`resources:Resources`)
- program configuration (see :numref:`user_configuration:Program configuration`)

.. graphviz:: graphviz/work-flow.gv
   :caption: A sample workflow showing the different types of algorithm supported by Phlex (see :numref:`functional_programming:Higher-order functions supported by Phlex` for a list of the supported algorithms).
             Solid arrows show the flow of data through the graph.
             Dotted lines indicate communication of data through the IO system.
             The *Driver* is configured to process all *spills* in the specified ROOT input files.
             The *provide* algorithms are configured to read *SimHits* associated with *spills* from the ROOT input files and a single *Geometry* object from the GDML file.
             For each *spill*, an *unfold* algorithm is configured to create a sequence of *Waveforms* objects, creating one *Waveforms* object in each *APA*.
             A *transform* algorithm is run on each of the *Waveforms* objects to create a *ClampedWaveforms* object.
             A *fold* algorithm is run on each of the *ClampedWaveforms* objects in a *spill* to create a *SummedWaveforms* object for the *spill*.
             Each *Waveforms*, *ClampedWaveforms*, and *SummedWaveforms* objects are written to a ROOT output file, each one associated with the appropriate *spill* or *APA*.
             This workflow also shows a *filter* algorithm selecting only "high energy" *Waveforms*, and an *observe* algorithm creating a histogram from them, which is written to a ROOT analysis file.
             Note that in this workflow the names *spill* and *APA* are not special to the Phlex framework; they are names (hypothetically) chosen by the experiment.
   :name: workflow

.. toctree::
   :maxdepth: 2

   scheduling
   data_organization
   algorithms
   sources_sinks
   resources
   user_configuration
