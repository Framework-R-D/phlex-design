Definitions
===========

.. glossary::
   :sorted:

   APA
       Anode Plane Assembly: physical far-detector unit comprising thousands of wires as a planar slice

   Algorithm
       A decomposed unit of processing; a user-defined function registered for execution by the framework.

   Algorithmic origin
       The unique identifier, version, and configuration of the algorithm responsible for the creation of a specific data product.

   Ancestral data
       The data used as input to the algorithm that produced the data at issue.

   Backward compatibility (persisted data)
       The ability to construct the in-memory representation of data from persisted information derived from an earlier in-memory representation of those data.

   CRP
       Charge Readout Plane: physical far-detector unit placed on either the top or bottom edge of an APA

   Configuration canonical form
       The result of a transformation on a configuration that preserves the semantics of the configuration.
       All semantically identical configurations shall have the same canonical form.
       Any given configuration shall have exactly one canonical form (which may be syntactically identical to the original configuration).

   Configured higher-order function (CHOF)
       An entity created when registering an algorithm with the framework.
       The registration includes the algorithm's input/output data product requirements, the algorithm's resource requirements, and the higher-order function to which the algorithm serves as an operator.

   Data family
       A collection of data sets (e.g.):

       - examples of data families in art: collection of Runs, collection of Events, and collection of SubRuns
       - other examples of data families: collection of calibration intervals of the same type, collection of geometry/alignment intervals of the same type, collection of APAs, collection of trigger primitives, collection of beam spills

       The type of an element in a data family is called a category.
       For example, the category of a family of Events is the type Event.
       An object of type Event is a data set.

   Data family hierarchy
       A hierarchy of data families (e.g. run contains subrun contains event).

   Data model
       A set of mechanisms enabling the definition, creation, identification, and organization of data products, as well as the relationships among them.
       The data model also specifies the mechanism for reading and writing persistable data products.

   Data product
       An object of data the framework can provide as an input to a user-defined algorithm, or that can be produced as an output of an algorithm.

   Data product set
       A mathematical set of data products that is identifiable by the framework and used to determine which data products serve as inputs to an algorithm.

   Deterministic
       Identical inputs produce identical results via identical behavior.

   Framework ecosystem
       Ensemble of software delivered with the application framework.

   Integration test
       A test that verifies the behavior of multiple system components working in concert.

   Metadata
       Data that is ancillary to physics data.

   Module
       A compiled library that is dynamically loadable by the framework.

   Parentage
       The set of unique identifiers of each ancestral datum.

   Persistable data product
       A data product that can be read from and written to persistent storage.

   Provenance
       A description of how the data at issue were produced (parentage, configuration, library versions, and other information as specified by requirements).

   Reproducible
       Identical inputs produce identical results.

   Resource
       A non-data software or hardware component managed by the program that can be required by algorithms.

       Examples include:

       - CPU cores
       - CPU memory
       - GPU "cores"
       - GPU memory
       - Network
       - Thread-unsafe utilities
       - Inference servers
       - Databases

   Unit test
       A test that verifies the behavior of a small and self-contained system component in isolation from other system components.

   Validation
       Did it do what we needed it to do (requirements)

   Verification
       Did it do what we intended it to do (code)
