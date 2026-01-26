Glossary
========

.. glossary::
    :sorted:

    Algorithm
        A user-defined function registered for execution by the framework.

        Algorithms often serve as operators in a higher-order function.

    APA
        **A**\ node **P**\ lane **A**\ ssembly, a physical far-detector unit comprising thousands of wires as a planar slice.

    Backward compatibility (persisted data)
        The ability to construct the in-memory representation of data from persisted information derived from an earlier in-memory representation of those data.

    CHOF
        *See* :term:`Configured higher-order function`

    Configured higher-order function
        An entity created when registering an algorithm with the framework.

        The registration includes the algorithm's input/output data product requirements, the algorithm's resource requirements, and the higher-order function to which the algorithm serves as an operator.

    Data layer
        A group of data cells each of which has the same data layer label.

        A data layer is one node in a data-layer hierarchy.
        The top layer of the hierarchy is always the `Job`.
        All other layers are defined by the combination of the configuration of the framework job and the layers defined in the input data.
        By contrast, in *art* the data layers are `Run`, `Subrun`, and `Event`.

    Data-layer hierarchy
        A hierarchy of data layers.

        A data-layer hierarchy is an acyclic graph of relationships of logical containment.
        The top layer of the hierarchy is always the `Job`.
        All other layers in the hierarchy are defined by the combination of the configuration of the framework job and the layers defined in the input data.
        In *art*, the data-layer hierarchy is `Run`\ -\ `Subrun`\ -\ `Event`.

    Concrete data product
        An object managed by the framework and for which provenance information is recorded.

        A concrete data product is produced by an algorithm (or is recovered from storage by a provider) and can be passed as an input to other algorithms.
        Concrete data products determine the flow of execution of the graph of CHOFs configured in a framework program.

    Concrete data-product type
        The programming-language type of a concrete data product.

    Data-product concept
        An abstraction that captures the computationally-relevant aspects of a physical data product.

        A data-product concept can be modeled by multiple concrete data-product types.
        A physical data product can also be abstracted differently by distinct data-product concepts.

    Physical data product
        A real-world entity like a raw detector readout, some calibration information, or a derived physics quantity, such as a collection of hits.

        A particular physical data product may be represented by multiple concrete data products, possibly of different concrete data-product types.

    Data cell
        A grouping of data products that is identifiable by the framework [#data-set]_.

        Each data cell has a data layer label, which indicates the type of the data cell.
        All the data cells of a given data layer are of the same type.
        All the data cells of a given data layer are identifiable by the same type of index, and each is identified by a unique index value.
        In *art*, individual `Run` objects, `Subrun` objects and `Event` objects are examples of data cells.

    .. The framework ecosystem is referenced in one of the requirements, so it's included in the glossary for even the conceptual design

    Framework ecosystem
        The ensemble of software delivered with the application framework.

    Index set
        A mathematical set that provides the indexes of an indexed family.

        For the family :math:`a = [a_1, a_2, \dots, a_n] = \family{a}`, the index set :math:`\isetdefault` is the set :math:`\{1,\ 2,\ \dots,\ n\}`.

    Indexed family
        A collection of elements identified by an index from an index set.

        The term family is often used as a shorthand for indexed family.
        In *art* there was no direct representation of families.
        However, the sequence of *Events* processed by a given job is an example of a family of data cells.
        In a single *art* module, the sequence of event data products looked up by the module across all events is an example of a family of data products.

    Metadata
        Data that is ancillary to physics data.

    Module
        A compiled library that is dynamically loadable by the framework.

    Provenance
        A description of how the data were produced.

        Examples include product parentage, job configuration, and library versions.

    Reproducible
        Identical inputs produce identical results.

    Resource
        A non-data software or hardware component managed by the program that can be used by algorithms.

        Examples include:

        - CPU cores
        - CPU memory
        - GPUs
        - Network
        - Thread-unsafe utilities
        - Inference servers
        - Databases


    .. The following definitions are needed for the supporting/subsystem design

    .. only:: not conceptual_design_only

        Algorithmic origin
            The unique identifier, version, and configuration of the algorithm responsible for the creation of a specific data product.

        Ancestral data
            The data used as input to the algorithm that produced the data at issue.

        Configuration canonical form
            The result of a transformation on a configuration that preserves the semantics of the configuration.

            All semantically identical configurations shall have the same canonical form.
            Any given configuration shall have exactly one canonical form (which may be syntactically identical to the original configuration).

        Data model
            A set of mechanisms enabling the definition, creation, identification, and organization of data products, as well as the relationships among them.

            The data model also specifies the mechanism for reading and writing persistable data products.

        Parentage
            The set of unique identifiers of each ancestral datum.

        Persistable data product
            A data product that can be read from and written to persistent storage.

.. rubric:: Footnotes

.. [#data-set] In earlier documentation, the term *data product set* was used for this concept.
