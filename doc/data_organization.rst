Data organization
=================

This section provides a conceptual overview of *data products*, *data product sets*, *data set categories*, and *data set families*, as defined in :dune:`11.1 Definition of data products,11.2 Creation of data sets,11.3 Definition of data families,11.4 Definition of data family hierarchies`.
It aims to establish a mental model for how all of these facilitate scientific workflows without delving into implementation specifics.

Data products represent things like raw detector readouts, calibration information, and derived physics quantities.
We call these kinds of things represented by data products *conceptual data products*.
Data product types are the programming language representations of conceptual data products.
A data set category is an experiment-defined level of aggregation of data products, e.g. run, subrun, spill, or an interval of validity for some flavor of calibration.
A data product set is a collection of data products, associated with a data set category.
A data set family is a sequence of data product sets that are in the same category.
:numref:`data-organization` illustrates the relationships between all of these.

.. graphviz:: graphviz/data-organization.gv
   :caption: An example of some possible data product hierarchies.
   :name: data-organization

Data products
-------------

Conceptual Nature
^^^^^^^^^^^^^^^^^

Data products are entities that encapsulate processed or raw data, of all kinds, separate from the algorithms that create them :dune:`46 Algorithm-Data Separability`.
They serve as the primary medium for communication between algorithms, ensuring seamless data exchange across processing steps :dune:`1.1 Algorithm Communication Via Data Products`.
They are associated with (rather than containing) metadata and provenance information that describe how the data products were created :dune:`51 Provenance discovery`.
They are not tied to specific hardware or algorithm implementations, ensuring independence and reproducibility :dune:`52.2 Independence from unique hardware characteristics`.
They enable the framework to present data produced by one algorithm to subsequent algorithms, supporting iterative and chained processing workflows :dune:`9 Presenting data to subsequent algorithms`.


Structure and Representation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The in-memory layout of a data product is determined by its type in the specified programming language.
Phlex does not require that the in-memory representation of a data product shall be the same as its persistent representation :dune:`2 Data Product Representation`.
In general, a single conceptual data product can be represented in multiple data product types, including types for different supported programming languages.

The framework provides the ability to determine the memory footprint of each data product :dune:`24.3 Specification of algorithm's CPU memory usage`.

Defining Data Product Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Data product types are not defined by the framework.
Framework users are expected to define their own data products :dune:`11.1 Definition of data products`.

.. todo:: Sketch how users define data product types.

Data categories, families, and data-product sets
------------------------------------------------

Hierarchical and Flexible Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Data products are organized into user-defined datasets, families, and hierarchies, supporting varying levels of granularity (e.g., tracks, raw data, calibration outputs) :dune:`11.1 Definition of data products,11.2 Creation of data sets,11.3 Definition of data families`.
They can be unfolded into finer-grained units, enabling detailed analysis or reprocessing at different scales :dune:`20 Algorithms independent of framework interface`.
This provides the ability to process data too large to fit into memory at one time :dune:`13 Process collections of unconstrained size`.

Data product management
-----------------------

Management of the data products returned by an algorithm is taken over by the framework.
Readonly access to input data products is provided to algorithms :dune:`51 Provenance discovery,58 Thread-safe design for algorithms`.
Data products that are intended to be written out are sent to the IO system as soon as they are created :dune:`26.2 Optimize memory management for data products`.
Data products are removed from memory as soon as they are no longer needed as input to another algorithm :dune:`26.2 Optimize memory management for data products`.


Data product metadata
---------------------

Each data product is associated with a specific set of metadata.
These metadata record information about how the data product was created, to allow that created to be reproducible.
The metadata are also used in data product lookup, to specificy which data products are to be provided as inputs to an algorithm.


Fields required for identifying data products from which to create a sequence:

- data category
- product type
- phase
- creator
- name





Data-product lookup policies
----------------------------
