****************
Technical Design
****************

The graph below depicts the relationships between the framework subsystems.
A subsystem is a cohesive set of components that together provide a well-defined set of functionalities to the rest of the framework.
Each subsystem has a set of responsibilities and a well-defined interface to be used by the other subsystems and plugins that need to use the provided functionality.
In the figure, the ellipses represent subsystems, and the arrows indicate what other subsystems each subsystem depends upon.
The subsystems have no circular dependencies.
The diagram shows a box that identifies the *core framework subsystems*, another that identifies the *external subsystems*, and a third one that shows examples of *plugins*.

.. graphviz:: subsystems.gv

The figure organizes all Phlex subsystems into three groups, with two standalone cross-cutting nodes.

The 14 core subsystems and their dependency arrows show the following structure:

- **Framework Application** provides the executables run by the user.

- **Task Management** is the most widely connected core node.
  It depends on *IO*, *Data Product Management*, *Metadata Management*, *Monitoring*,
  *Random Numbers*, *Resource Management*, *Error Handler*, and *Signal Handler*,
  reflecting its role as the runtime scheduler and executor.
- **Data Product Management** depends on *Metadata Management* and *Data Modeling*,
  establishing a chain: products carry metadata and are described by the data model.
- **IO** depends on *Data Modeling*, meaning the IO subsystem uses the data model to
  know how to read and write products.
- **Metadata Management** depends on both *Data Modeling* and *Registration*, as metadata
  must be anchored to typed products and registered algorithms.
- **Plugin Management** and **Data Modeling** both depend on *Registration* and
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
   ch_subsystem_design/data_product_management
   ch_subsystem_design/data_modeling
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
