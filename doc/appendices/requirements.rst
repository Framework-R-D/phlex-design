**********************
Framework requirements
**********************

**Requirements norm**: Baseline 1 (created March 03, 2025)

.. important::

    All stakeholder requirements approved by DUNE are listed here for convenience.
    However, the requirements recorded in the `DUNE framework Jama Connect project <https://fnal-prod.jamacloud.com/perspective.req#/projects/63/dashboard/63>`_ are authoritative and take precedence over any unintentional variations below.

=======================
Conceptual requirements
=======================


.. req:: Algorithm Decomposability
    :collapse:
    :id: DUNE 1
    :jama: 1 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14536
    :notes: This is ID #01 from the original DUNE document.
    :status: Approved
    :tags: General, Original

    The framework shall allow the execution of multiple algorithms.

- See :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`

.. req:: Algorithm Communication Via Data Products
    :collapse:
    :id: DUNE 111
    :jama: 111 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14847
    :status: Approved
    :tags: General, Reproducibility, Provenance

    The framework shall mediate communication between algorithms via data products.

- See :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`, :numref:`ch_conceptual_design/data_organization:Conceptual Nature`, :numref:`ch_conceptual_design/registration:Framework registration`

.. req:: Data Product Representation
    :collapse:
    :id: DUNE 2
    :jama: 2 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14539
    :notes: This is ID #02 from the original DUNE document.
    :status: Approved
    :tags: Original, General, Accelerators

    The framework shall separate the persistent representation of data products from their in-memory representations as seen by algorithms.

- See :numref:`ch_conceptual_design/data_organization:Structure and Representation`

.. req:: Full utilization of DUNE computing resources
    :collapse:
    :id: DUNE 8
    :jama: 8 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14548
    :notes: This is ID #05 from the original DUNE document.
    :status: Approved
    :tags: General, Original, Reproducibility

    The framework shall run on widely-used scientific computing systems in order to fully utilize DUNE computing resources.

- See :numref:`introduction:Portability`

.. req:: Algorithm hardware requirements
    :collapse:
    :id: DUNE 9
    :jama: 9 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14549
    :notes: This is ID #06 from the original DUNE document.
    :status: Approved
    :tags: General, Original, Reproducibility

    The framework shall provide an API that allows users to express hardware requirements of the algorithms.

- See :numref:`ch_conceptual_design/registration:Framework registration`

.. req:: Algorithms can use a GPU
    :collapse:
    :id: DUNE 11
    :jama: 11 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14551
    :status: Approved
    :tags: General, Accelerators, Reproducibility

    The framework shall support running algorithms that require a GPU.

- See :numref:`introduction:Portability`

.. req:: Support for multiple programming languages
    :collapse:
    :id: DUNE 14
    :jama: 14 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14554
    :notes: This is ID #07 from the original DUNE document.
            If DUNE decides that additional languages should be supported in the future, a specific requirement can be added for that language as a sub-requirement.
    :status: Approved
    :tags: Original, General

    The framework shall support the invocation of algorithms written in multiple programming languages.

- See :numref:`introduction:Programming languages`

.. req:: Support C++ algorithms
    :collapse:
    :id: DUNE 81
    :jama: 81 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14663
    :status: Approved
    :tags: General

    The framework shall support the invocation of algorithms written in C++.

- See :numref:`introduction:Programming languages`

.. req:: Support Python algorithms
    :collapse:
    :id: DUNE 82
    :jama: 82 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14664
    :status: Approved
    :tags: General

    The framework shall support the invocation of algorithms written in Python.

- See :numref:`introduction:Programming languages`

.. req:: Persist user-defined metadata
    :collapse:
    :id: DUNE 17
    :jama: 17 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14557
    :notes: This is ID #08 from the original DUNE document.
    :status: Approved
    :tags: General, Original

    The framework shall provide user-accessible persistence of user-defined metadata.

- See :numref:`ch_conceptual_design/writers:Data-Product Writers`

.. req:: Framework shall read its own output files
    :collapse:
    :id: DUNE 19
    :jama: 19 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14560
    :notes: This is ID #10 from the original DUNE document.
    :status: Approved
    :tags: Physics Analysis, Original

    The framework shall provide the ability to read a framework-produced file as input to a subsequent framework job so that the physics data are equivalent to the physics data obtained from a single execution of the combined job.

- See :numref:`ch_conceptual_design/writers:Data-Product Writers`

.. req:: Presenting data to subsequent algorithms
    :collapse:
    :id: DUNE 20
    :jama: 20 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14562
    :status: Approved
    :tags: Physics Analysis

    The framework shall present data produced by an already executed algorithm to each subsequent, requesting algorithm.

- See :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`, :numref:`ch_conceptual_design/data_organization:Conceptual Nature`

.. req:: Mix input streams
    :collapse:
    :id: DUNE 21
    :jama: 21 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14563
    :notes: This is ID #11 from the original DUNE document.
    :status: Approved
    :tags: Physics Analysis, Original

    The framework shall support the creation of data sets composed of data products derived from data originating from disparate input sources.

- See :numref:`ch_conceptual_design/driver:Framework Driver`.

.. req:: Flexible data units
    :collapse:
    :id: DUNE 22
    :jama: 22 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14580
    :notes: This is ID #12 from the original DUNE document.
    :status: Approved
    :tags: Original, Flexible Processing Unit (FPU)

    The framework shall support flexibly defined, context-aware processing units to address the varying granularity necessary for processing different kinds of data.

- See :numref:`introduction:Requirements Process and Framework Selection`, :numref:`introduction:Flexibility`

.. req:: Definition of data products
    :collapse:
    :id: DUNE 85
    :jama: 85 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14693
    :status: Approved
    :tags: Flexible Processing Unit (FPU)

    The framework shall provide the ability for user-level code to define data products.

- See :numref:`ch_conceptual_design/data_organization:Data organization`, :numref:`ch_conceptual_design/data_organization:Defining Data Product Types`, :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`

.. req:: Creation of data sets
    :collapse:
    :id: DUNE 86
    :jama: 86 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14696
    :status: Approved
    :tags: Flexible Processing Unit (FPU)

    The framework shall provide the ability for user-level code to create new data sets.

- See :numref:`ch_conceptual_design/data_organization:Data organization`, :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`

.. req:: Definition of data families
    :collapse:
    :id: DUNE 87
    :jama: 87 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14697
    :status: Approved
    :tags: Flexible Processing Unit (FPU)

    The framework shall provide the ability for user-level code to define data families.

- See :numref:`ch_conceptual_design/data_organization:Data organization`, :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`

.. req:: Definition of data family hierarchies
    :collapse:
    :id: DUNE 88
    :jama: 88 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14698
    :status: Approved
    :tags: Flexible Processing Unit (FPU)

    The framework shall provide the ability for user-level code to define hierarchies of data families.

- See :numref:`ch_conceptual_design/data_organization:Data organization`, :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`

.. req:: Process collections of unconstrained size
    :collapse:
    :id: DUNE 25
    :jama: 25 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14584
    :notes: This originates from ID #16 from the original DUNE document.
    :status: Approved
    :tags: Original, Flexible Processing Unit (FPU)

    The framework shall support processing of collections that are too large to fit into memory at one time.

- See :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`

.. req:: Unfolding data products
    :collapse:
    :id: DUNE 33
    :jama: 33 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14593
    :notes: This is ID #58 from the original DUNE document.
    :status: Approved
    :tags: Memory management, Original, Flexible Processing Unit (FPU)

    The framework shall allow the unfolding of data products into a sequence of finer-grained data products.

- See :numref:`ch_conceptual_design/hofs/partitioned_unfolds:Partitioned Unfolds`

.. req:: Access to external data sources
    :collapse:
    :id: DUNE 35
    :jama: 35 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14595
    :notes: This is ID #47 from the original DUNE document.
            By "external data sources," we mean "data sources **other than** framework-readable data files containing detector readout or simulated physics data."
    :status: Approved
    :tags: Original, Services

    The framework shall support access to external data sources.

- See :numref:`ch_conceptual_design/registration:Data Products from Different Data Categories`, :numref:`ch_conceptual_design/resources:Resources`

.. req:: Calibration database algorithms
    :collapse:
    :id: DUNE 40
    :status: Approved
    :jama: 40 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14600
    :notes: This is ID #68 as proposed to DUNE.
    :tags: Services

    The framework shall support algorithms that provide data from calibration databases.

- See :numref:`ch_conceptual_design/data_organization:Data organization`, :numref:`ch_conceptual_design/driver:Framework Driver`, :numref:`ch_conceptual_design/resources:Resources`.

.. req:: Algorithms independent of framework interface
    :collapse:
    :id: DUNE 43
    :status: Approved
    :jama: 43 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14608
    :notes: This is ID #48 from the original DUNE document.
    :tags: Services, Original

    The framework shall support the registration of algorithms that are independent of framework interface.

- See :numref:`introduction:Usability`, :numref:`introduction:Framework Independence`, :numref:`ch_conceptual_design/data_organization:Data categories, families, and data-product sets`, :numref:`ch_conceptual_design/algorithms:Algorithms`

.. req:: Safely executing thread-safe and non-thread-safe algorithms
    :collapse:
    :id: DUNE 45
    :status: Approved
    :jama: 45 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14611
    :notes: This is ID #26 from the original DUNE document.
    :tags: Original, Concurrency and multithreading

    The framework shall safely execute user algorithms declared to be non-thread-safe along with those declared to be thread-safe.

- See :numref:`ch_conceptual_design/resources:Limited Resources`

.. req:: Resource specification for the program
    :collapse:
    :id: DUNE 47
    :status: Approved
    :jama: 47 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14613
    :notes: This is ID #28 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall enable the specification of resources required by the program.

- See :numref:`ch_conceptual_design/resources:Resources`

.. req:: Specification of user-defined resources
    :collapse:
    :id: DUNE 149
    :status: Approved
    :jama: 149 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15871
    :tags: Resource management

    The framework shall enable the specification of user-defined resources required by the program.

- See :numref:`ch_conceptual_design/resources:User-defined Resources`

.. req:: Resource specification for algorithms
    :collapse:
    :id: DUNE 52
    :status: Approved
    :jama: 52 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14620
    :notes: This is ID #33 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall enable the specification of resources required by each algorithm.

- See :numref:`ch_conceptual_design/resources:Resources`

.. req:: Serial access to a thread-unsafe resource
    :collapse:
    :id: DUNE 145
    :status: Approved
    :jama: 145 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15856
    :tags: Concurrency and multithreading, Original, Resource management

    The framework shall permit algorithm authors to specify that the algorithm requires serial access to a thread-unsafe resource.

- See :numref:`ch_conceptual_design/resources:Limited Resources`

.. req:: Specification of algorithm's user-defined resources
    :collapse:
    :id: DUNE 155
    :status: Approved
    :jama: 155 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15891
    :tags: Resource management

    The framework shall enable the specification of user-defined resources required by the algorithm.

- See :numref:`ch_conceptual_design/resources:User-defined Resources`

.. req:: Resource-based algorithm concurrency
    :collapse:
    :id: DUNE 50
    :status: Approved
    :jama: 50 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14618
    :notes: This is ID #31 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall dynamically schedule algorithms to execute efficiently according to the availability of each algorithm's required resources.

- See :numref:`ch_conceptual_design/resources:Resources`

.. req:: Optimize memory management for data products
    :collapse:
    :id: DUNE 142
    :status: Approved
    :jama: 142 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15847
    :notes: Optimization means making the data products available for the shortest period of time possible for all algorithms that require them.
            The framework, however, may need to run in series multiple algorithms requiring those data products if those algorithms would run out of resources if run concurrently.
    :tags: Resource management

    The framework shall optimize the memory management of data products.

- See :numref:`ch_conceptual_design/data_organization:Data product management`, :numref:`ch_conceptual_design/hofs/partitioned_unfolds:Registration Interface`

.. req:: Composable workflows using GPU algorithms and CPU algorithms
    :collapse:
    :id: DUNE 54
    :status: Approved
    :jama: 54 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14622
    :notes: This is ID #36 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall support composable workflows that use GPU algorithms along with CPU algorithms.

- See :numref:`ch_conceptual_design/resources:GPUs`

.. req:: Specification of data products required by an algorithm
    :collapse:
    :id: DUNE 65
    :status: Approved
    :jama: 65 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14634
    :notes: This is ID #63 as proposed to DUNE.
    :tags: Registration

    The framework shall support the specification of data products required as input by an algorithm.

- See :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`, :numref:`ch_conceptual_design/registration:Framework registration`

.. req:: Specification of data products created by an algorithm
    :collapse:
    :id: DUNE 156
    :status: Approved
    :jama: 156 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15897
    :tags: Registration

    The framework shall support the specification of data products created as output by an algorithm.

- See :numref:`ch_conceptual_design/scheduling:Topology of the data-flow graph`, :numref:`ch_conceptual_design/registration:Framework registration`

.. req:: One configuration per framework execution
    :collapse:
    :id: DUNE 69
    :status: Approved
    :jama: 69 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14638
    :notes: This is ID #44 from the original DUNE document.
    :tags: Original, Configuration

    The framework shall accept exactly one configuration per program execution.

- See :numref:`ch_conceptual_design/program_configuration:Program Configuration`

.. req:: Framework configuration language
    :collapse:
    :id: DUNE 72
    :status: Approved
    :jama: 72 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14641
    :notes: This is ID #60 as proposed to DUNE.
    :tags: Configuration

    The framework shall provide the ability to configure the execution of a framework program at runtime using a human-readable language.

- See :numref:`ch_conceptual_design/program_configuration:Program Configuration`

.. req:: I/O plugins
    :collapse:
    :id: DUNE 73
    :status: Approved
    :jama: 73 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14642
    :notes: This is ID #50 from the original DUNE document.
            Data includes physics data and metadata (both user-provided and framework metadata).
            The goal is to enable non-framework developers to implement an IO backend without needing to modify the framework itself.
    :tags: Data I/O layer, Original

    The framework shall provide a public API that enables the implementation of a concrete IO backend for a specific persistent storage format.

- See :numref:`ch_conceptual_design/providers:Data-Product Providers`, :numref:`ch_conceptual_design/writers:Data-Product Writers`

.. req:: Data I/O backward compatibility
    :collapse:
    :id: DUNE 76
    :status: Approved
    :jama: 76 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14645
    :notes: This is ID #54 from the original DUNE document.
            Backward compatibility means that new code is able to read data produced by older versions of the framework.
    :tags: Original, Data I/O layer, Backwards compatibility

    The framework IO subsystem shall support backward compatibility across versions, subject to policy decisions on deprecation provided by DUNE.

- See :numref:`ch_conceptual_design/providers:Data-Product Providers`

.. req:: Algorithm invocation with data products from multiple data sets
    :collapse:
    :id: DUNE 89
    :status: Approved
    :jama: 89 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14705
    :tags: Flexible Processing Unit (FPU)

    The framework shall allow a single invocation of an algorithm with data products from multiple data sets.

- See :numref:`ch_conceptual_design/registration:Algorithms with Multiple Input Data Products`

.. req:: Algorithm invocation with data products from adjacent data sets
    :collapse:
    :id: DUNE 91
    :status: Approved
    :jama: 91 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14713
    :tags: Flexible Processing Unit (FPU)

    The framework shall support the invocation of an algorithm with data products belonging to adjacent data sets.

- See :numref:`ch_conceptual_design/registration:Data Products from Adjacent Data-Product Sets`

.. req:: User-defined adjacency
    :collapse:
    :id: DUNE 92
    :status: Approved
    :jama: 92 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14714
    :tags: Flexible Processing Unit (FPU)

    The framework shall support user code that defines adjacency of data sets within a data family.

- See :numref:`ch_conceptual_design/registration:Data Products from Adjacent Data-Product Sets`

.. req:: Algorithm invocation with data products from multiple data families
    :collapse:
    :id: DUNE 113
    :status: Approved
    :jama: 113 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14937
    :tags: Flexible Processing Unit (FPU)

    The framework shall allow a single invocation of an algorithm with data products from multiple data families.

- See :numref:`ch_conceptual_design/registration:Data Products from Different Data Categories`

.. req:: Specification of algorithm output FPUs
    :collapse:
    :id: DUNE 90
    :status: Approved
    :jama: 90 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14706
    :notes: To implement this requirement, the algorithm should not know where its created data products are going--that is something that will be specified at the plugin level (perhaps by configuration).
    :tags: Flexible Processing Unit (FPU)

    The framework shall support the user specification of which data family to place the data products created by an algorithm.

- See :numref:`ch_conceptual_design/registration:Framework Registration`

.. req:: Algorithm-Data Separability
    :collapse:
    :id: DUNE 110
    :status: Approved
    :jama: 110 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14845
    :tags: General

    The data objects exchanged among algorithms shall be separable from those algorithms.

- See :numref:`ch_conceptual_design/data_organization:Conceptual Nature`

.. req:: Provenance discovery
    :collapse:
    :id: DUNE 121
    :status: Approved
    :jama: 121 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15293
    :tags: Provenance

    The framework shall enable users to discover the provenance of data products.

- See :numref:`introduction:Framework philosophy`, :numref:`ch_conceptual_design/data_organization:Conceptual Nature`, :numref:`ch_conceptual_design/data_organization:Data product management`, :numref:`ch_conceptual_design/data_organization:Data product identification`.

.. req:: Framework recording of metadata for reproduction of output data
    :collapse:
    :id: DUNE 28
    :status: Approved
    :jama: 28 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14588
    :notes: This is ID #18 from the original DUNE document.
    :tags: Original, Reproducibility, Provenance

    The framework shall record metadata to output enabling the reproduction of the processing steps used to produce the data recorded in that output.

- See :numref:`ch_conceptual_design/writers:Data-Product Writers`

.. req:: Reproducibility of data products
    :collapse:
    :id: DUNE 122
    :status: Approved
    :jama: 122 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15294
    :tags: Reproducibility, Provenance

    The framework shall support the reproduction of data products from the provenance stored in the output.

- See :numref:`ch_conceptual_design/data_organization:Data product identification`.

.. req:: Reproducibility with pseudo-random numbers
    :collapse:
    :id: DUNE 36
    :status: Approved
    :jama: 36 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14596
    :notes: This is ID #22 from the original DUNE document.
    :tags: Original, Reproducibility, Provenance

    The framework shall provide a facility to produce random numbers enabling algorithms to create reproducible data in concurrent contexts.

- See :numref:`ch_conceptual_design/resources:Random Number Resource`

.. req:: Thread-safe design for algorithms
    :collapse:
    :id: DUNE 130
    :status: Approved
    :jama: 130 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15742
    :tags: Concurrency and multithreading

    The framework shall facilitate the development of thread-safe algorithms.

- See :numref:`ch_preliminaries/functional_programming:Functional programming`, :numref:`ch_conceptual_design/data_organization:Data product management`

.. req:: Composing configurations of framework components
    :collapse:
    :id: DUNE 133
    :status: Approved
    :jama: 133 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15775
    :tags: Configuration

    The framework shall support executing programs configured by composing configurations of separate components.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Graceful shutdown of framework program
    :collapse:
    :id: DUNE 134
    :status: Approved
    :jama: 134 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15787
    :notes: A *graceful shutdown* refers to a framework program that completes the processing of all in-flight data, safely closes all open input and output files, cleans up connections to external entities (e.g. databases), etc. before the program ends.
            This ensures that no resources are left in ill-defined states and that all output files are readable and valid.
            An important example of this is when a batch job exceeds a time limit and the grid system sends a signal to shutdown the job, which should end gracefully.
    :tags: Error handling

    The framework shall attempt a graceful shutdown by default.

- See :numref:`introduction:Usability`

=======================
Supporting requirements
=======================

.. req:: Shut down upon unmet algorithm hardware requirements
    :collapse:
    :id: DUNE 13
    :status: Approved
    :jama: 13 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14553
    :tags: General

    The framework shall shut down if the platform fails to meet each specified hardware requirement.

.. req:: Emit diagnostic upon unmet algorithm hardware requirements
    :collapse:
    :id: DUNE 112
    :status: Approved
    :jama: 112 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14885
    :tags: General

    The framework shall emit a diagnostic message for each hardware requirement the platform fails to meet.

.. req:: Provide instructions for writing algorithms in supported languages
    :collapse:
    :id: DUNE 16
    :status: Approved
    :jama: 16 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14556
    :tags: Documentation

    The framework documentation shall provide instructions for writing framework-executable algorithms in supported languages.

.. req:: Data product I/O independence
    :collapse:
    :id: DUNE 24
    :status: Approved
    :jama: 24 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14583
    :notes: This is ID #14 from the original DUNE document.
    :tags: Original, Data I/O layer

    The framework shall support reading from disk only the data products required by a given algorithm.

.. req:: Read collections of unconstrained size
    :collapse:
    :id: DUNE 32
    :status: Approved
    :jama: 32 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14592
    :notes: This is ID #57 from the original DUNE document.
            Very large data collections could be 10s of GBs in size, and the memory budget may be a maximum of a few GBs.
    :tags: Original, Memory management

    The framework shall support the reading of collections too large to hold in memory.

.. req:: Write collections of unconstrained size
    :collapse:
    :id: DUNE 120
    :status: Approved
    :jama: 120 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15288
    :tags: Original, Memory management

    The framework shall support the writing of collections too large to hold in memory.

.. req:: Configuration comparison
    :collapse:
    :id: DUNE 98
    :status: Approved
    :jama: 98 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14724
    :tags: Configuration

    The framework shall provide the ability to compare two configurations.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Record execution environment
    :collapse:
    :id: DUNE 30
    :status: Approved
    :jama: 30 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14590
    :notes: This is ID #20 from the original DUNE document.
    :tags: Original, Reproducibility, Provenance

    The framework shall record the job's execution environment.

.. req:: User-selectable list of recordable execution environment components
    :collapse:
    :id: DUNE 100
    :status: Approved
    :jama: 100 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14730
    :tags: Reproducibility, Provenance

    The framework shall provide the list of recordable components of the execution environment.

.. req:: Save user-selected execution environment components
    :collapse:
    :id: DUNE 101
    :status: Approved
    :jama: 101 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14731
    :tags: Reproducibility, Provenance

    The framework shall save each execution-environment description selected by the user from the framework-provided-list.

.. req:: Record user-selected items from the shell environment
    :collapse:
    :id: DUNE 123
    :status: Approved
    :jama: 123 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15480
    :tags: Provenance

    The framework shall record user-selected items from the shell environment.

.. req:: User-provided execution environment information
    :collapse:
    :id: DUNE 124
    :status: Approved
    :jama: 124 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15482
    :tags: Provenance

    The framework shall record labelled execution environment information provided by the user.

.. req:: Maximum memory usage
    :collapse:
    :id: DUNE 31
    :status: Approved
    :jama: 31 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14591
    :notes: This is ID #59 from the original DUNE document.
            The maximum memory available is a static quantity that can apply to (a) a job using an entire node with all of its available RSS, and (b) a job using a specific grid slot with a limit on the RSS.
            It is assumed that the operating system and C++/Python runtimes are not already enforcing this limit.
    :tags: Original, Memory management, Resource management

    The framework shall gracefully shut down if the program attempts to exceed a configured memory limit.

.. req:: Diagnostic message when exceeding memory limit
    :collapse:
    :id: DUNE 137
    :status: Approved
    :jama: 137 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15802
    :tags: Memory management, Error handling

    The framework shall emit a diagnostic message if the program attempts to exceed the configured maximum memory.

.. req:: Algorithm code versioning and build information
    :collapse:
    :id: DUNE 39
    :status: Approved
    :jama: 39 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14599
    :notes: This is ID #25 from the original DUNE document.
    :tags: Original, Reproducibility, Provenance

    The framework shall have an option to record build information, including the source code version, associated with each algorithm.

.. req:: Intra-algorithm concurrency and multi-threading
    :collapse:
    :id: DUNE 46
    :status: Approved
    :jama: 46 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14612
    :notes: This is ID #27 from the original DUNE document.
            It is the responsibility of the algorithm author to ensure that any parallelism libraries used can work compatibly with those used by the framework itself.
    :tags: Original, Concurrency and multithreading

    The framework shall allow algorithms to use the same parallelism mechanisms the framework uses to schedule the execution of algorithms.

- See :numref:`ch_subsystem_design/task_management:Task management`

.. req:: Specification of maximum number of CPU threads
    :collapse:
    :id: DUNE 146
    :status: Approved
    :jama: 146 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15865
    :tags: Resource management

    The framework shall enable the specification of the maximum number of CPU threads permitted by the program.

.. req:: Specification of maximum allowed CPU memory
    :collapse:
    :id: DUNE 147
    :status: Approved
    :jama: 147 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15866
    :tags: Resource management, Memory management

    The framework shall enable the specification of the maximum CPU memory allowed by the program.

.. req:: Specification of GPU resources
    :collapse:
    :id: DUNE 148
    :status: Approved
    :jama: 148 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15869
    :tags: Resource management

    The framework shall enable the specification of GPU resources required by the program.

.. req:: Specification of algorithm's maximum number of CPU threads
    :collapse:
    :id: DUNE 152
    :status: Approved
    :jama: 152 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15883
    :tags: Resource management

    The framework shall enable the specification of the maximum number of CPU threads permitted by the algorithm.

- See :numref:`ch_conceptual_design/registration:Framework registration`

.. req:: Specification of algorithm's CPU memory usage
    :collapse:
    :id: DUNE 154
    :status: Approved
    :jama: 154 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15889
    :tags: Resource management, Memory management

    The framework shall enable the specification of an algorithm's expected CPU memory usage.

- See :numref:`ch_conceptual_design/data_organization:Structure and Representation`

.. req:: Specification of algorithm's GPU resources
    :collapse:
    :id: DUNE 153
    :status: Approved
    :jama: 153 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15886
    :tags: Resource management

    The framework shall enable the specification of GPU resources required by the algorithm.

.. req:: Remote GPU algorithm support
    :collapse:
    :id: DUNE 42
    :status: Approved
    :jama: 42 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14607
    :notes: This is ID #70 as proposed to DUNE.
    :tags: Services

    The framework shall support algorithms that perform calculations using a remote GPU.

.. req:: Local GPU algorithm support
    :collapse:
    :id: DUNE 41
    :status: Approved
    :jama: 41 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14602
    :notes: This is ID #69 as proposed to DUNE.
    :tags: Services

    The framework shall support algorithms that perform calculations using a local GPU.

.. req:: Logging resource usage per algorithm invocation
    :collapse:
    :id: DUNE 48
    :status: Approved
    :jama: 48 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14614
    :notes: This is ID #29 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall support logging the usage of a specified resource for each algorithm using the resource.

.. req:: Elapsed time information
    :collapse:
    :id: DUNE 60
    :status: Approved
    :jama: 60 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14629
    :notes: This is ID #66 as proposed to DUNE.
            This option is intended to capture wall-clock time and not CPU time.
            If more granular reporting of CPU vs. IO time is required, dedicated profiling tools like VTune or Linaro Forge should be used.
    :tags: General, Resource management

    The framework shall have an option to provide elapsed time information for each algorithm executed in a framework program.

- See :numref:`ch_subsystem_design/monitoring:Monitoring and reporting`

.. req:: Efficient execution of algorithms requiring access to a network resource
    :collapse:
    :id: DUNE 144
    :status: Approved
    :jama: 144 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15854
    :notes: An example of efficient execution is an algorithm requiring network resource does not occupy a core that can do other work while the algorithm "waits" for the resource to respond.
    :tags: Concurrency and multithreading, Original, Resource management

    The framework shall efficiently execute a graph of algorithms where at least one algorithm requires access to a network resource.

.. req:: Optimize availability of external resources
    :collapse:
    :id: DUNE 143
    :status: Approved
    :jama: 143 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15848
    :notes: Examples of external resources include local GPUs, remote inference servers, and databases.
            This requirement also serves as a replacement for a former requirement: "The framework MUST be able to broker/cache service replies that might be common to multiple instances of algorithms (reduce load on external service/disk/memory/network/...). E.g. a request for a calibration constant that is common among all FPUs in a run.
            Not every instance of an algorithm should trigger an actual request to the central database providing these."
    :tags: Resource management

    The framework shall optimize the availability of external resources.

.. req:: Efficient execution of algorithms with specific CPU memory requirements
    :collapse:
    :id: DUNE 150
    :status: Approved
    :jama: 150 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15874
    :tags: Resource management, Memory management

    The framework shall efficiently execute a graph of algorithms where at least one algorithm specifies a required amount of CPU memory.

.. req:: Efficient execution of algorithms with specific GPU memory requirements
    :collapse:
    :id: DUNE 151
    :status: Approved
    :jama: 151 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15875
    :tags: Resource management, Memory management

    The framework shall efficiently execute a graph of algorithms where at least one algorithm specifies a required amount of GPU memory.

.. req:: Emit message describing data flow of program
    :collapse:
    :id: DUNE 68
    :status: Approved
    :jama: 68 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14637
    :notes: This is ID #64 as proposed to DUNE.
    :tags: Concurrency and multithreading

    The framework shall have an option to emit a description of the data flow of a configured program without executing the workflow.

.. req:: Emit message stating algorithm resource requirements
    :collapse:
    :id: DUNE 56
    :status: Approved
    :jama: 56 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14625
    :notes: This is ID #38 from the original DUNE document.
    :tags: Original, Concurrency and multithreading, Resource management

    The framework shall have an option to emit a message stating the resources required by each algorithm of a configured program without executing the workflow.

.. req:: Monitoring global memory use
    :collapse:
    :id: DUNE 59
    :status: Approved
    :jama: 59 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14628
    :notes: This is ID #67 as proposed to DUNE.
    :tags: General, Memory management, Resource management

    The framework shall be able to report the global memory use of the framework program at user-specified points in time.

- See :numref:`ch_subsystem_design/monitoring:Monitoring and reporting`

.. req:: Framework-independent message logging
    :collapse:
    :id: DUNE 61
    :status: Approved
    :jama: 61 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14630
    :notes: This is ID #65 as proposed to DUNE.
    :tags: General

    The framework shall support a logging solution that is usable in an algorithm without that algorithm explicitly relying on the framework.

- See :numref:`ch_subsystem_design/logging:Logging`

.. req:: Configuration validation
    :collapse:
    :id: DUNE 64
    :status: Approved
    :jama: 64 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14633
    :notes: This is ID #42 from the original DUNE document.
    :tags: Original, Configuration

    The framework shall validate an algorithm's configuration against specifications provided at registration time.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Algorithm configuration schema availability
    :collapse:
    :id: DUNE 67
    :status: Approved
    :jama: 67 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14636
    :notes: This is ID #43 from the original DUNE document.
    :tags: Original, Configuration

    The framework shall have an option to emit an algorithm's configuration schema in human-readable form.

- See :numref:`ch_subsystem_design/configuration:Semantic structure of the configuration`

.. req:: Eager validation of algorithm configuration
    :collapse:
    :id: DUNE 70
    :status: Approved
    :jama: 70 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14639
    :notes: This is ID #45 from the original DUNE document.
            Validation includes any reading, parsing, canonicalizing, and checking against applicable schemata.
    :tags: Original, Configuration

    The framework shall validate the configuration of each algorithm before that algorithm processes data.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`, :numref:`ch_subsystem_design/configuration:Semantic structure of the configuration`.

.. req:: I/O backend for ROOT
    :collapse:
    :id: DUNE 74
    :status: Approved
    :jama: 74 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14643
    :notes: This is ID #51 from the original DUNE document.
    :tags: Data I/O layer, Original

    The framework ecosystem shall support a ROOT IO backend.

- See :numref:`ch_conceptual_design/data_organization:Conceptual Nature`.

.. req:: I/O backend for HDF5
    :collapse:
    :id: DUNE 141
    :status: Approved
    :jama: 141 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15838
    :tags: Data I/O layer, Original

    The framework ecosystem shall support an HDF5 IO backend.

- See :numref:`ch_conceptual_design/data_organization:Conceptual Nature`.

.. req:: Data product backward compatibility
    :collapse:
    :id: DUNE 106
    :status: Approved
    :jama: 106 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14796
    :tags: Data I/O layer, Backwards compatibility

    The framework's IO subsystem shall support backward compatibility of data products.

.. req:: Metadata backward compatibility
    :collapse:
    :id: DUNE 107
    :status: Approved
    :jama: 107 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14797
    :notes: Metadata here can include user-visible (including user-provided) metadata, and framework metadata, which may not be user-visible but is necessary for framework operation.
    :tags: Data I/O layer, Backwards compatibility

    The framework's IO subsystem shall support backward compatibility of metadata.

.. req:: Configurable data compression
    :collapse:
    :id: DUNE 77
    :status: Approved
    :jama: 77 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14646
    :notes: This is ID #55 from the original DUNE document.
    :tags: Original, Data I/O layer

    The framework IO subsystem shall allow user-configuration of compression settings for each concrete IO implementation.

.. req:: User-configurable output file rollover
    :collapse:
    :id: DUNE 78
    :status: Approved
    :jama: 78 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14647
    :notes: This is ID #56 from the original DUNE document.
    :tags: Original, Data I/O layer

    The framework shall support user-configurable rollover of output files.

.. req:: Output file rollover due to number of data sets in data family
    :collapse:
    :id: DUNE 109
    :status: Approved
    :jama: 109 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14806
    :notes: Some examples include limiting the output file to contain data for: (a) 1 subrun ("subrun" is the user-specified family) (b) 100 spills ("spill" is the user-specified family) (c) 10 blobs ("blob" is the user-specified family)
    :tags: Data I/O layer

    The framework shall have an option to rollover output files according to a configurable limit on the number of data sets in a user-specified data family.

.. req:: Output file rollover due to file size
    :collapse:
    :id: DUNE 138
    :status: Approved
    :jama: 138 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15826
    :tags: Data I/O layer

    The framework shall have an option to rollover output files according to a configurable limit on output-file size.

.. req:: Output file rollover due to user-defined quantities
    :collapse:
    :id: DUNE 139
    :status: Approved
    :jama: 139 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15830
    :notes: An example of an aggregated value of a user-derived quantity is the number of protons on target (POTs).
    :tags: Data I/O layer

    The framework shall have an option to rollover output files according to a configurable limit on the aggregated value of a user-derived quantity.

.. req:: Output file rollover due to file lifetime
    :collapse:
    :id: DUNE 140
    :status: Approved
    :jama: 140 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15832
    :tags: Data I/O layer

    The framework shall have an option to rollover output files according to a configurable limit on the time the file has been open.

.. req:: ProtoDUNE single-phase raw data
    :collapse:
    :id: DUNE 116
    :status: Deferred
    :jama: 116 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15239
    :notes: ProtoDUNE single-phase was used in run 1.
            For this requirement, the framework *ecosystem* is responsible for processing run 1 data (e.g. the framework might not read the run 1 data directly, but a translation program might first prepare the run 1 data for reading within the framework).
    :tags: Backwards compatibility, Data I/O layer

    The framework ecosystem shall support processing ProtoDUNE single-phase raw data.

.. req:: ProtoDUNE dual-phase raw data
    :collapse:
    :id: DUNE 117
    :status: Deferred
    :jama: 117 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15240
    :notes: ProtoDUNE dual-phase was used in run 1.
            For this requirement, the framework *ecosystem* is responsible for processing run 1 data (e.g. the framework might not read the run 1 data directly, but a translation program might first prepare the run 1 data for reading within the framework).
    :tags: Backwards compatibility, Data I/O layer

    The framework ecosystem shall support processing ProtoDUNE dual-phase raw data.

.. req:: ProtoDUNE II horizontal-drift raw data
    :collapse:
    :id: DUNE 118
    :status: Approved
    :jama: 118 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15242
    :tags: Backwards compatibility, Data I/O layer

    The framework ecosystem shall support processing ProtoDUNE II horizontal-drift raw data.

.. req:: ProtoDUNE II vertical-drift raw data
    :collapse:
    :id: DUNE 119
    :status: Approved
    :jama: 119 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15244
    :tags: Backwards compatibility, Data I/O layer

    The framework ecosystem shall support processing ProtoDUNE II vertical-drift raw data.

.. req:: Framework configuration persistency
    :collapse:
    :id: DUNE 27
    :status: Approved
    :jama: 27 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14587
    :notes: This is ID #17 from the original DUNE document.
            This requirement is in support of documenting and reproducing previous results.
    :tags: Original, Configuration, Reproducibility, Provenance

    The framework shall provide an option to persist the configuration of each framework execution to the output of that execution.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Independence from unique hardware characteristics
    :collapse:
    :id: DUNE 63
    :status: Approved
    :jama: 63 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=14632
    :notes: This is ID #62 as proposed to DUNE.
    :tags: Reproducibility, Provenance

    The framework shall operate independently of unique characteristics of existing hardware.

- See :numref:`introduction:Portability`, :numref:`ch_conceptual_design/data_organization:Conceptual Nature`

.. req:: Command line interface (CLI)
    :collapse:
    :id: DUNE 125
    :status: Approved
    :jama: 125 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15710
    :tags: Configuration

    The framework shall provide a command-line interface that allows the setting of configuration parameters.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Support local configuration changes
    :collapse:
    :id: DUNE 126
    :status: Approved
    :jama: 126 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15715
    :tags: Configuration

    The framework shall support the use of local configuration changes with respect to a separate complete configuration to modify the execution of a program.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Configuration tracing
    :collapse:
    :id: DUNE 127
    :status: Approved
    :jama: 127 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15718
    :tags: Configuration

    The framework configuration system shall have an option to provide diagnostic information for an evaluated configuration, including origins of final parameter values.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Configuration language single point of maintenance
    :collapse:
    :id: DUNE 128
    :status: Approved
    :jama: 128 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15723
    :notes: This must be met by each configuration language.
    :tags: Configuration

    The language used for configuring a framework program shall include features for maintaining hierarchical configurations from a single point of maintenance.

- See :numref:`ch_subsystem_design/configuration:Mechanics of Configuration Specification`

.. req:: Enable identification of data sets containing chunked data products
    :collapse:
    :id: DUNE 129
    :status: Approved
    :jama: 129 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15737
    :tags: Provenance, Chunkification

    The framework shall record metadata identifying data sets where the framework took special measures to process data collections of unconstrained size.

.. req:: Framework build flags
    :collapse:
    :id: DUNE 131
    :status: Approved
    :jama: 131 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15744
    :tags: Debugging

    The framework build system shall support options that enable debugging executed code.

.. req:: Floating-point environment
    :collapse:
    :id: DUNE 132
    :status: Approved
    :jama: 132 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15746
    :tags: Reproducibility, Error handling, Debugging

    The framework shall allow the per-execution setting of the float-point environment to control the handling of IEEE floating-point exceptions.

.. req:: Graceful shutdown for uncaught exception
    :collapse:
    :id: DUNE 135
    :status: Approved
    :jama: 135 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15788
    :tags: Error handling

    The framework shall by default attempt a graceful shutdown upon receiving an uncaught exception from user algorithms.

.. req:: Graceful shutdown for received signal
    :collapse:
    :id: DUNE 136
    :status: Approved
    :jama: 136 https://fnal-prod.jamacloud.com/perspective.req?projectId=63&docId=15789
    :tags: Error handling

    The framework shall by default attempt a graceful shutdown when receiving a signal.
