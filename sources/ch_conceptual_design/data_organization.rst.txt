Data Organization
=================

This section provides a conceptual overview of *data products*, *data cells*, *data layers*, and *data-layer hierarchies*, as defined in :need:`DUNE 85` :need:`DUNE 86` :need:`DUNE 87` :need:`DUNE 88`.
In addition, we discuss *data-product families* and *data-cell families*.
This section aims to establish a mental model for how all of these concepts facilitate scientific workflows without delving into implementation specifics.

:term:`Physical data products <Physical data product>` are real-world entities like raw detector readouts, calibration information :need:`DUNE 40`, or derived physics quantities, such as collections of hits.
:term:`Data-product concepts <Data-product concept>` are abstractions that capture the computationally-relevant aspects of physical data products.
:term:`Concrete data products <Concrete data product>` are programming-language objects that represent physical data products; :term:`concrete data-product types <Concrete data-product type>` are the types of these objects.
:numref:`data-product-relationships` shows how each of these aspects are related to each other for a real-world collection of hits.

.. graphviz:: data-products.gv
   :caption: The data-product terminology for the physical data product "hits", and the relations between the terms.
   :name: data-product-relationships

A :term:`data layer` is an experiment-defined level of aggregation of data products.
Some example data layers are `Run`, `Subrun`, `Spill`, and an interval of validity for some flavor of calibration.
Each Phlex job includes a `Job` data layer at the top of the data-layer hierarchy.
A data cell is a collection of concrete data products, associated with a data layer.
A data-cell family is a family of data cells that are in the same data layer.
The `Job` layer always includes a single data cell.
:numref:`data-organization` illustrates the relationships between all of these.

.. graphviz:: data-organization.gv
   :caption: The data organization corresponding to part of :numref:`workflow`.
             The framework-provided `Job` data layer and three different user-defined (not special to the Phlex framework) data layers are shown: `Run`, `Spill`, and `APA`.
             Rectangles with labels :math:`\textsf{Run}_i`, :math:`\textsf{Spill}_{i,j}`, and :math:`\textsf{APA}_{i,j,k}` represent data cells.
             The pale green rectangles show two data-cell families; these are identified as families because they are the result of executing the :mathfunc:`unfold(into_apas)` node shown in :numref:`workflow`.
             A solid line from one data cell to another data cell represents a logical association between the two data cells.
             The bottom rectangle shows that :math:`\textsf{Waveforms}_{1,1,1}` is in the data cell :math:`\textsf{APA}_{1,1,1}`, etc.
             Each pale purple rectangle indicates the data-product family created by unfolding each :product:`SimDepos` object as shown in :numref:`workflow`.
   :name: data-organization

In :numref:`data-organization`, the `Run` data layer exists, but as no algorithm in :numref:`workflow` requires any data products from a `Run` data cell, the framework does not create any data-cell families corresponding to the `Run`.

Data Products
-------------

Data products are entities that encapsulate processed or raw data, of all kinds, separate from the algorithms that create them :need:`DUNE 110`.
They serve as the primary medium for communication between algorithms, ensuring seamless data exchange across processing steps :need:`DUNE 111`.
They are associated with (rather than containing) metadata and provenance information that describe how the data products were created :need:`DUNE 121`.
They are not tied to specific hardware or algorithm implementations, ensuring independence and reproducibility :need:`DUNE 63`.
They are also not tied to any specific IO back end, but must support reading and writing with both ROOT :need:`DUNE 74` and HDF5 :need:`DUNE 141`.
They enable the framework to present data produced by one algorithm to subsequent algorithms, supporting iterative and chained processing workflows :need:`DUNE 20`.

Structure and Representation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The in-memory layout of a data product is determined by its type in the specified programming language.
Phlex does not require that the in-memory representation of a data product shall be the same as its persistent representation :need:`DUNE 2`.
In general, a single conceptual data product can be represented by multiple programming language types.
This includes representing a single conceptual data product in multiple supported programming languages.

The framework provides the ability to determine the memory footprint of each data product :need:`DUNE 154`.

Defining Data Product Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Data product types are not defined by the framework.
Framework users are expected to define their own data product types :need:`DUNE 85`.


Data Layers, Data Cells, and Families
-------------------------------------

As illustrated in :numref:`data-organization`, data products are organized into user-defined data cells, families, layers, and hierarchies, supporting varying levels of granularity :need:`DUNE 86` :need:`DUNE 87` :need:`DUNE 88`.
They can be unfolded into finer-grained units, enabling detailed analysis or reprocessing at different scales :need:`DUNE 43`.
This provides the ability to process data too large to fit into memory at one time :need:`DUNE 25`.

Data Product Management
-----------------------

Management of the data products returned by an algorithm is taken over by the framework.
Read-only access to input data products is provided to algorithms :need:`DUNE 121` :need:`DUNE 130`.
Read-only access to a data product must not mutate it.
Data products that are intended to be written out are sent to the IO system as soon as they are created :need:`DUNE 142`.
Data products are removed from memory as soon as they are no longer needed for writing or as input to another algorithm :need:`DUNE 142`.

Data Product Identification
---------------------------

Each data product is associated with a specific set of metadata describing the algorithms and configurations used in their creation.
These metadata allow that creation to be reproducible :need:`DUNE 122`.
The metadata are stored along with the data in the framework output file, and the IO interface allows access to the metadata :need:`DUNE 121`.

The data products created by an algorithm are associated with metadata that identify the algorithm that created them.
Such metadata include:

- the *creator*, the name of the algorithm that created the data product
- an identifier for the *data cells* with which the data product is associated (e.g. `Spill`, `Run`, `Calibration Interval`, or other experiment-defined layer)
- the *processing phase*, an identifier for the job in which the data product was created
- an individual *name* for the data product (which may be empty), to distinguish between multiple products of the same type created by the same algorithm.

In addition to these metadata, a data product is also specified by its *type*.

The metadata are stored in the framework output file, and the IO interface allows access to the metadata :need:`DUNE 121`.

The metadata are also used in data product lookup, to specify which data products are to be provided as inputs to an algorithm.
The algorithms are configured to identify the inputs in which they are interested by selecting on  any of the metadata defined above, as well as by the programming language types of their inputs.
