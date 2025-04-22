Data organization
=================

This section provides a conceptual overview of *data products*, *data product sets*, *data set categories*, and *data set families*, as defined in :dune:`11.1 Definition of data products,11.2 Creation of data sets,11.3 Definition of data families,11.4 Definition of data family hierarchies`.
It aims to establish a mental model for how all of these facilitate scientific workflows without delving into implementation specifics. 

Data products represent things like raw detector readouts, calibration information, and derived physics quanties.
We call these kinds of things represented by data products *conceptual data products*.
Data product types are the programming language representations of conceptual data products.
A data set category is an experiement-defined level of aggregation of data products, e.g. run, subrun, spill, or an interval of validity for some flavor of calibration.
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

Data products are distinct entities that encapsulate processed or raw data, separate from the algorithms :dune:`46 Algorithm-Data Separability`.
They serve as the primary medium for communication between algorithms, ensuring seamless data exchange across processing steps :dune:`1.1 Algorithm Communication Via Data Products`.   
They are associated with metadata and provenance information that describe how the data products were created :dune:`51 Provenance discovery`. 
They are not tied to specific hardware or algorithm implementations, ensuring independence and reproducibility :dune:`52.2 Independence from unique hardware characteristics`.  
They enable the framework to present data produced by one algorithm to subsequent algorithms, supporting iterative and chained processing workflows :dune:`9 Presenting data to subsequent algorithms`. 

Hierarchical and Flexible Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Data products are organized into user-defined datasets, families, and hierarchies, supporting varying levels of granularity (e.g., tracks, raw data, calibration outputs) :dune:`11.1 Definition of data products`, :dune: `11.2 Creation of data sets`, and :dune:`11.3 Definition of data families`.
They can be unfolded into finer-grained units, enabling detailed analysis or reprocessing at different scales :dune:`20 Algorithms independent of framework interface`.

Defining Data Product Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo:: Describe what defining data products means.

Data product types are not defined by the framework.
Framework users are expected to define their own data products :dune:`11.1 Definition of data products`.



Structure and Representation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The in-memory layout of a data product is determined by its type in the specified programming language. 
Phlex does not require that the in-memory representation of a data product shall be the same as its persistent representation :dune:`2 Data Product Representation`.
In some cases, it may even be advantageous to have multiple in-memory representations, and therefore multiple data products types, for the same conceptual data product. 

Data products maintain a separation between their persistent (on-disk) and in-memory representations, allowing flexibility in how they are stored versus how algorithms interact with them.


Abstract Types and Dependencies
Data products can represent abstract types (e.g., raw data, calibration datasets) and may have dependencies on other data products, which must be explicitly defined (STK #145, #154).

However, they are not required to manage dependencies themselves, as the framework handles these relationships (STK #145).

.. todo:: Should we include a section on how to have different representations of the same data product?

Data product identifiers
^^^^^^^^^^^^^^^^^^^^^^^^

Fields required for identifying data products from which to create a sequence:

- data category
- product type
- phase
- creator
- name

Lifecycle and Processing 
^^^^^^^^^^^^^^^^^^^^^^^

Creation and Usage   
Data products are created by algorithms and can be concrete (e.g., new datasets) or abstract (e.g., processed outputs), with their creation and usage governed by user specifications (STK #85, #154).   

They support processing of large-scale collections, requiring the ability to handle data that exceeds memory limits through chunking or streaming (STK #25). 

Memory Management   
The framework optimizes the memory footprint of data products, ensuring they are available only as long as needed by algorithms, minimizing resource usage (STK #142, #154).   

Rapid release of data products after use is prioritized to maintain efficient memory utilization, especially in concurrent workflows (STK #150). 

Reusability and Reproducibility   
Data products must support reproducibility, carrying sufficient metadata to recreate them without reliance on specific hardware (STK #122, #63).   

They enable provenance tracking, recording the processing steps and configurations used in their creation (STK #121). 

Access and Integration 
^^^^^^^^^^^^^^^^^^^^^^

External Data Integration   
Data products can incorporate data from external sources (e.g., calibration databases), requiring a design that supports integration with diverse inputs (STK #21, #40).   

They must be accessible to remote systems, such as GPUs, ensuring efficient serialization for distributed processing (STK #141, #142). 

Framework-Mediated Access   
The framework mediates access to data products, ensuring that algorithms can request specific data without needing to manage concurrency or thread safety (STK #111, #145).   

CHANGE: Data products are not required to be thread-safe, as the framework handles concurrency challenges (e.g., serializing access for non-thread-safe algorithms) (STK #145). 


Constraints and Challenges
----------------------------
Resource Constraints
Data products must operate within defined memory and CPU/GPU usage limits, necessitating efficient designs that avoid excessive resource consumption (STK #147, #154).

The framework ensures graceful handling of resource limits, but data products must be lightweight to support this (STK #31).

Concurrency and Workflows
Data products are used in workflows involving both CPU and GPU algorithms, requiring compatibility with diverse processing environments without managing concurrency themselves (STK #154).

The framework optimizes scheduling to minimize memory usage, but data products must support rapid access and release to enable this (STK #142).

Identification and Traceability
Unique identification of data products (e.g., through metadata) is a challenge, especially for chunked or distributed datasets, but is necessary for provenance and reproducibility (STK #129).

Data products must carry metadata to track their origin and processing history, supporting scientific validation (STK #121).

Data categories, families, and data-product sets
------------------------------------------------


Data-product lookup policies
----------------------------
