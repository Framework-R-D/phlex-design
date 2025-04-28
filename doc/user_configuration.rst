Program configuration
=====================

Conceptual nature and constraints
---------------------------------

The configuration of a particular invocation of a Phlex application may be factorized into:

#. The mechanics of how the configuration is specified
#. The way the configuration is accessed programmatically by both framework and user code.
#. The semantic structure of the configuration as it relates to the structure and operation of the application

The mechanics of configuration specification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configuration of the Phlex application may be effectuated by:

#. The ingestion of a configuration document in a specific language, or
#. The use of one or more command-line options to the invocation of Phlex.

Command line options will always be applied after—and with higher priority than—a configuration document.

A given Phlex invocation will be configured by no more than one configuration document, although that document may be compiled from more than one source (such as included files).

Phlex will support configuration documents written in precisely one of the following languages: JSON, Jsonnet, YAML, or FHiCL.

Programmatic access to the configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to preserve the framework-independent nature of user algorithms, the appropriate section of configuration will be made available to the registration code in the form of a hierarchical configuration object in the appropriate programming language (C++ or Python).
The user is then responsible for ensuring that relevant configuration information is passed to the user algorithm.

Customizable facilities will exist to enable the presentation of configuration items not only as simple types, but as complex strucutres in the appropriate programming language.

Users will not be able generally to access live configuration outside the scope of their own algorithm, although "framework-level" configuration will be viewable from registration code, as will configuration metadata attached to any input data registered for the algorithm.

Semantic structure of the configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The semantic structure of the configuration is constrained by the common features of the permitted document languages, and of the C++ and Python programmatic representations.
At the top level, the configuration document will be organized as a single associative array of key-value pairs, with the key being a scalar string identifier, and the value being either a scalar, a sequence of values, or a nested associative array.
The configuration for a specific user algorithm or framework feature category will be represented by a (possibly empty) associative array.

Neither sequences nor associative arrays are constrained to be homogeoneous.

Facilities will exist to constrain and/or validate configurations for particular framework feature categories or user code by specifying a schema; when used, any issues with the validity of user-provided configuration will be flagged at the time of evaluation of the configuration, rather than being left to the author of the registration code and/or algorithm.
