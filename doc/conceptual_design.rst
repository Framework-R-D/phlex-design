*****************
Conceptual Design
*****************

.. admonition:: Purpose

   The conceptual design is not a reference manual; it is a high-level description of how the framework aims to satisfy the stakeholder requirements (see :numref:`Appendix %s <appendices/requirements:Framework requirements>`).
   The audience for the conceptual design is the physicist, algorithm author, or framework program runner.
   More detailed design aspects in support of the conceptual model are given in :numref:`Chapter %s <supporting_design:Supporting design>`.

Phlex adopts the data-flow approach discussed in :numref:`ch_preliminaries/data_flow:Data flow with families`.
Instead of expressing scientific workflows as monolithic functions to be executed, workflows are factorized into composable algorithms that operate on data products passed among them :need:`DUNE 1`, :need:`DUNE 111`, :need:`DUNE 20`.
These algorithms then serve as operators to higher-order functions (HOFs) that operate on *data-product families*.

To guide the discussion of Phlex's conceptual model, we refer to :numref:`workflow`, which shows a small fictitious workflow that creates vertices from simulated energy deposits.
Various framework aspects are demonstrated by that figure:

   *data-flow graph*
      The data-flow graph is formed by ingesting the configuration file and recording the data-product dependencies required of each algorithm (see :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`).

   *data-product flow*
      Data products and data-product sets (see :numref:`ch_conceptual_design/data_organization:Data organization`) are passed along graph edges.
      As mentioned in :numref:`ch_preliminaries/data_flow:Data Flow with families`, the data passed between HOFs are expressed as families.
      :numref:`workflow` thus formally passes families (e.g. :math:`[\textit{GoodHits}_{jk}]`) between nodes [#flow]_.

   *framework driver*
      The driver instructs the framework what to process (see :numref:`ch_conceptual_design/driver:Framework Driver`).

      The driver in :numref:`workflow` is configured so that all `Spill`\ s in the specified ROOT input files are processed.

   *data-product providers*
      Data-product providers are framework components that provide data products from external entities to downstream user algorithms (see :numref:`ch_conceptual_design/providers:Data-Product Providers`).
      From a functional-programming perspective, they are transforms that map a data-product set to the data product within that set.

      In the workflow, one provider reads a :math:`\textit{SimDepos}` data product from each `Spill` in the ROOT input files, and the other reads a single :math:`\textit{Geometry}` corresponding to the `Job` from a GDML file.

   *HOFs and user-provided algorithms*
      Arguably the most important aspect of the framework is how user-provided algorithms are bound to HOFs and registered with the framework (see :numref:`ch_conceptual_design/algorithms:Algorithms`, :numref:`ch_conceptual_design/registration:Framework Registration` and :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions`).

      All seven HOFs supported by Phlex (see :numref:`hofs_in_phlex`) are used in :numref:`workflow`.
      For the main processing chain of creating vertices:

      - An *unfold* algorithm is configured to create a family of :math:`\textit{Waveforms}` objects—creating one :math:`\textit{Waveforms}` object per `APA`—from one :math:`\textit{SimDeps}` data product in each `Spill`.
      - A *transform* algorithm is run on each :math:`\textit{Waveforms}` objects to create a :math:`\textit{GoodHits}` object.
      - To make a :math:`\textit{GoodTracks}` data product, a *window* algorithm is applied to pairs of :math:`\textit{GoodHits}` objects that come from adjacent *APA*\ s.
      - Lastly, another *transform* algorithm operates on the :math:`\textit{GoodTracks}` data products to produce vertices.

      There are additional parts of the graph that are not directly related to creating vertices:

      - A *fold* algorithm is executed over the :math:`\textit{GoodHits}` data products to sum the hit energy (i.e. :math:`\textit{TotalHitEnergy}`) across all `APA`\ s for a given `Spill`.
      - After a *filter* has been applied with the *predicate* :math:`\textit{high\_energy}`, an *observe* algorithm is used to fill a histogram with hit-related information from the :math:`\textit{GoodHits}` data products.

   *data-product writers*
      Data-product writers are plugins that write data products to framework outputs (see :numref:`ch_conceptual_design/writers:Data-Product writers`)

      Each of the five writers in :numref:`workflow` is responsible for writing to one or more output files.

   *resources*
      Most workflows require access to some external resource (see :numref:`ch_conceptual_design/resources:Resources`).

      The histogramming resource in :numref:`workflow` enables the *observe* algorithm to fill and write histograms to a ROOT analysis file.

Note that in this workflow, the names *Spill* and *APA* are not special to the Phlex framework; they are names (hypothetically) chosen by the experiment.
Each data product is also indexed, thus associating it with a particular data-product set (e.g. :math:`\textit{GoodHits}_{3,7}` denotes the :math:`\textit{GoodHits}` data product belonging to `APA` :math:`7` of `Spill` :math:`3`).

.. graphviz:: work-flow.gv
   :caption: A fictitious workflow showing how HOFs are used in a Phlex program.
             Each unshaded node represent a HOF bound to a user-defined algorithm, whose name is shaded in blue.
             Each user-defined algorithm operates on arguments received from the incoming arrows to the node: data products are passed along solid arrows; objects that provide access to resources are passed along dashed arrows.
             Whereas single-dotted lines indicate communication of data through the framework's IO system, double-dotted lines denote communication of data with entities not directly related to the framework.
             See text for workflow details.
   :name: workflow

.. toctree::
   :maxdepth: 2

   ch_conceptual_design/scheduling
   ch_conceptual_design/data_organization
   ch_conceptual_design/algorithms
   ch_conceptual_design/registration
   ch_conceptual_design/supported_hofs
   ch_conceptual_design/driver
   ch_conceptual_design/providers
   ch_conceptual_design/writers
   ch_conceptual_design/resources
   ch_conceptual_design/program_configuration

.. rubric:: Footnotes

.. [#flow] In practice, elements of the family, not the full family itself, will be passed from one node to another.
           For memory purposes, it is also likely that each element of the family is a lightweight reference (in C++, a pointer) to the data of relevance.
