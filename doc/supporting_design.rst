****************
Technical Design
****************


Phlex is divided into several *subsystems*, each of which is responsible for a specific aspect of the framework's functionality.
A subsystem is a set of libraries with a well-defined public interface.
One subsystem may make use of the functionality provided by another subsystem only through the public interface of the other subsystem.
The set of subsystems may have no circular dependencies.

Phlex also makes use of several *external subsystems*, which are not part of the Phlex codebase but are used by Phlex to provide certain functionality.
There are also several external systems that users of Phlex may interact with in the context of using Phlex, but which are not part of Phlex itself.

This chapter of the document enumerates the subsystems, and describes their responsibliities and interactions.

.. toctree::
   :maxdepth: 2

   ch_subsystem_design/framework_subsystems
   ch_subsystem_design/program_startup
   ch_subsystem_design/role_of_data_product_concepts
   ch_subsystem_design/application
   ch_subsystem_design/task_management
   ch_subsystem_design/data_modeling
   ch_subsystem_design/data_product_management
   ch_subsystem_design/metadata_management
   ch_subsystem_design/io
   ch_subsystem_design/plugin_management
   ch_subsystem_design/resource_management
   ch_subsystem_design/monitoring
   ch_subsystem_design/registration
   ch_subsystem_design/algo_description
   ch_subsystem_design/error_handler
   ch_subsystem_design/signal_handler
   ch_subsystem_design/random_numbers
   ch_subsystem_design/configuration
   ch_subsystem_design/logging

External Subsystems
===================

- Build
- CI
- Documentation
- Event display
- Workflow management
