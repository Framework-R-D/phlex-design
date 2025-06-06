Program Configuration
=====================

The application of a specific configuration to a particular invocation of Phlex may be factorized into:

#. The mechanics of how the configuration is specified and evaluated
#. The way the configuration is accessed programmatically by both framework and user code
#. The semantic structure of the configuration as it relates to the structure and operation of the application

Mechanics of Configuration Specification
----------------------------------------

Configuration of the Phlex application may be effectuated by:

#. The ingestion of a configuration document in a specific language, and/or
#. The use of one or more command-line options to the invocation of Phlex :need:`DUNE 125`

Command line options shall always be applied after—and with higher priority than—a configuration document.

A given Phlex invocation shall be configured by no more than one configuration document :need:`DUNE 69`, although that document may be compiled from more than one source (such as included files).

Phlex shall support configuration documents written in precisely one of the following languages: Jsonnet, YAML, JSON (as a strict subset of Jsonnet), or FHiCL :need:`DUNE 72`.

Phlex shall evaluate the entire configuration at program initialization time :need:`DUNE 70`, and configuration values shall be converted into a canonical form to facilitate meaningful comparison between two different evaluated configurations :need:`DUNE 98`.

Where supported by the input language and requested by the user, Phlex shall provide tracing, and other diagnostic information about the evaluation of the configuration :need:`DUNE 127`.

Phlex shall provide the option to persist its configuration to the execution's configured output data destination :need:`DUNE 27`.

For each supported configuration input language, Phlex shall support facilities enabling a single point of maintenance for configuration documents :need:`DUNE 128`, including (but not limited to):

* Specifically overriding configuration items specified earlier in the configuration document :need:`DUNE 126`, and
* The ability to compose a single configuration document from multiple distinct sources :need:`DUNE 133`.

Programmatic Access to the Configuration
----------------------------------------

In order to preserve the framework-independent nature of user algorithms, the appropriate section of configuration will be made available to the appropriate algorithm's registration code in the form of a hierarchical configuration object in the appropriate programming language (C++ or Python).
The author of that registration code is then responsible for ensuring that relevant configuration information is passed to the user algorithm itself.

Customizable facilities will exist to enable the presentation of configuration items not only as simple types, but as complex strucutres in the appropriate programming language.

Users will not be able generally to access live configuration outside the scope of their own algorithm, although "framework-level" configuration will be viewable from registration code, as will configuration metadata attached to any input data registered for the algorithm.

Semantic Structure of the Configuration
---------------------------------------

The semantic structure of the configuration is constrained by the common features of the permitted document languages, and of the C++ and Python programmatic representations.
At the top level, the configuration document will be organized as a single associative array of key-value pairs, with the key being a scalar string identifier, and the value being either a scalar, a sequence of values, or a nested associative array.
The configuration for a specific user algorithm or framework feature category will be represented by a (possibly empty) associative array.

Neither sequences nor associative arrays are constrained to be homogeoneous.

Facilities will exist to constrain and/or validate configurations for particular framework feature categories or user code by specifying a schema :need:`DUNE 64`; when used, any issues with the validity of user-provided configuration will be flagged at the time of evaluation of the configuration :need:`DUNE 70`, rather than being left to the author of the registration code and/or algorithm.
A human-readable representation of a specific validation schema will be provided by the framework upon request :need:`DUNE 67`.
