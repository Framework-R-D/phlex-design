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

   ch_technical_design/framework_subsystems
   ch_technical_design/program_startup
   ch_technical_design/role_of_data_product_concepts
   ch_technical_design/application
   ch_technical_design/task_management
   ch_technical_design/data_modeling
   ch_technical_design/data_product_management
   ch_technical_design/metadata_management
   ch_technical_design/io
   ch_technical_design/plugin_management
   ch_technical_design/resource_management
   ch_technical_design/monitoring
   ch_technical_design/registration
   ch_technical_design/algo_description
   ch_technical_design/error_handler
   ch_technical_design/signal_handler
   ch_technical_design/random_numbers
   ch_technical_design/configuration
   ch_technical_design/logging

External Subsystems
===================

- Build
- CI
- Documentation
- Event display
- Workflow management
