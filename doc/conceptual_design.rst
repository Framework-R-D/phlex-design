*****************
Conceptual Design
*****************

.. admonition:: Purpose

   The conceptual design is not a reference manual; it is a high-level description of how the framework aims to satisfy the stakeholder requirements (see :numref:`Appendix %s <appendices/requirements:Framework requirements>`).
   The audience for the conceptual design is the physicist, algorithm author, or framework program runner.
   More detailed design aspects in support of the conceptual model are given in :numref:`supporting_design:Supporting design`.

Phlex adopts the data-flow approach discussed in :numref:`ch_preliminaries/data_flow:Data flow with sequences`.
Instead of expressing scientific workflows as monolithic functions to be executed, workflows are factorized into composable algorithms that operate on data products passed among them :need:`DUNE 1`, :need:`DUNE 111`, :need:`DUNE 20`.
These algorithms then serve as operators to higher-order functions that operate on *data-product sequences*.

To guide the discussion of Phlex's conceptual model, we refer to the graph in  :numref:`workflow`, which illustrates various framework aspects:

- the data-flow graph itself (see :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`)
- data products and data-product sets as passed along graph edges (see :numref:`ch_conceptual_design/data_organization:Data organization`)
- user-provided algorithms such as transforms, folds, etc. (see :numref:`ch_conceptual_design/algorithms:Algorithms` and :numref:`ch_conceptual_design/hof_operators:HOF operators` )
- the framework driver (see :numref:`ch_conceptual_design/driver:Framework Driver`)
- data-product providers (see :numref:`ch_conceptual_design/providers:Data-Product Providers`), which are plugins that provide data products from external entities to downstream user algorithms (e.g. input from ROOT files)
- data-product writers (see :numref:`ch_conceptual_design/writers:Data-Product writers`), which are plugins that may write data products to an external entity (e.g. output files)
- resources (see :numref:`ch_conceptual_design/resources:Resources`)
- program configuration (see :numref:`ch_conceptual_design/user_configuration:Program configuration`)

.. graphviz:: work-flow.gv
   :caption: A sample workflow showing the different types of algorithm supported by Phlex (see :numref:`ch_preliminaries/functional_programming:Higher-order functions supported by Phlex` for a list of the supported algorithms).
             Solid arrows show the flow of data through the graph.
             Dotted lines indicate communication of data through the IO system.
             The *SpillDriver* (see section :numref:`ch_conceptual_design/driver:Framework Driver`) is configured to process all *spills* in the specified ROOT input files.
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

   ch_conceptual_design/scheduling
   ch_conceptual_design/data_organization
   ch_conceptual_design/algorithms
   ch_conceptual_design/registration
   ch_conceptual_design/hof_operators
   ch_conceptual_design/driver
   ch_conceptual_design/providers
   ch_conceptual_design/writers
   ch_conceptual_design/resources
   ch_conceptual_design/user_configuration
