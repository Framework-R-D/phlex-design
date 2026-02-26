****************
Technical Design
****************

The graph below depicts the relationships between the framework subsystems.
A *subsystem* is a cohesive set of components that together provide a well-defined set of functionalities to the rest of the framework.
Each subsystem has a set of responsibilities and a well-defined interface to be used by the other subsystems and plugins that need to use the provided functionality.
In the figure, the ellipses represent subsystems, and the arrows indicate what other subsystems each subsystem depends upon.
The subsystems have no circular dependencies.
The diagram shows a box that identifies the *core framework subsystems*, another that identifies the *external subsystems*, and a third one that shows examples of *plugins*.

.. graphviz:: subsystems.gv
   :caption: Phlex subsystems

The figure organizes all Phlex subsystems into three groups, with two standalone cross-cutting nodes.

The 14 core subsystems and their dependency arrows show the following structure:

- **Framework Application** provides the executables run by the user.
  It is responsible for creating and executing the workflow graph.

- **Task Management** provides the runtime scheduler and executor.
  It depends on `oneTBB Flow Graph <https://oneapi-spec.uxlfoundation.org/specifications/oneapi/v1.3-rev-1/elements/onetbb/source/flow_graph>`_.
  It provides the nodes that are used to represent CHOFs in the workflow.
  It also provides the types that are used to represent the workflow graph as a whole.

- **Data Modeling** provides the mechanism by which users define data product types, and the tools used to translate between in-memory representations of data product concepts.

- **Data Product Management** provides the mechanisms to organize and identify data products.
  This includes the types used to represent data cells, data product families, and data layers.
  It also provides the mechanisms by which the lifetime of data products are managed.
  It provides the mechanisms used for data product lookup (based on data product metadata).

- **Metadata Management** provides the mechanisms by which framework-defined metadata are associated with data products, data cells, data product families, data layers, and jobs.
  It also provides the facilities by which users can define their own metadata types and associate them with the appropriate entities, listed above.
  It also provides the mechanisms by which the metadata associated with one of these items can be retrieved.


- **IO** provides the mechanisms by which data products and relevant metadata are read and written to persistent storage.
  It is unclear exactly what the responsibilities of the IO subsystem are.
  It is unclear what the relationship betwen the IO subsystem and FORM is.

- **Plugin Management** depend on *Registration* and
  *Monitoring*, contributing discovered plugins and data-model events to those subsystems
  respectively.
  
- **Resource Management** depends solely on *Monitoring*, reporting resource utilization.

- **Algorithm Description** depends on *Registration*, as descriptions are derived from
  what is registered.
- *Error Handler*, *Signal Handler*, *Random Numbers*, and *Monitoring and Reporting* are
  leaf or near-leaf nodes with no outward edges within the core cluster.

Two cross-cutting subsystems sit outside all clusters because every other subsystem may depend on them:

- **Configuration** — provides configurability for both the core subsystems and plugins.
- **Logging** — initializes the logging mechanism that can be used by both the core subsystems and plugins.


Three representative plugin categories are shown — *User Algorithms*, *IO Backends*, and
*Data Management* (e.g., RUCIO) — with an ``etc.`` node indicating extensibility.
As a group, plugins depend on **Configuration** (to configure each plugin) and **Logging**
(to emit messages), but are otherwise decoupled from core internals.



Five externally-managed concerns — *Build*, *CI*, *Documentation*, *Event Display*, and
*Workflow Management* — are grouped separately.
Of these, only **Workflow Management** has a cross-edge: a dotted *"Makes use of"* arrow
from the workflow manager to the core subsystems cluster, indicating that a workflow system
drives framework application execution but is not architecturally part of the framework
itself.

.. toctree::
   :maxdepth: 2

   ch_subsystem_design/task_management
   ch_subsystem_design/data_modeling
   ch_subsystem_design/data_product_management
   ch_subsystem_design/metadata_management
   ch_subsystem_design/io
   ch_subsystem_design/registration
   ch_subsystem_design/plugin_management
   ch_subsystem_design/resource_management
   ch_subsystem_design/application
   ch_subsystem_design/configuration
   ch_subsystem_design/algo_description
   ch_subsystem_design/error_handler
   ch_subsystem_design/signal_handler
   ch_subsystem_design/random_numbers
   ch_subsystem_design/monitoring
   ch_subsystem_design/logging

External Subsystems
===================

- Build
- CI
- Documentation
- Event display
- Workflow management
