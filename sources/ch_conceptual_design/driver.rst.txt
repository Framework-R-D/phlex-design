Framework Driver
================

In imperative programming languages, control flow structures (such as loops) are provided by the language.
Users of the language are not able to create new control flow facilities.
Previous frameworks contained framework-defined *event loops* that provided the control flow for a framework program.
Users of these frameworks were not able to create alternate control flow structures.

In the functional programming paradigm, control flow is provided by higher-order functions.
Users of such languages are able to create new control flow structures by writing new higher-order functions.
In Phlex, a framework *driver* is a higher-order function that provides the control flow for a framework program.
Phlex provides a variety of framework drivers and also allows users to register their own drivers with the framework, to support different processing needs, e.g. :need:`DUNE 21` and :need:`DUNE 40`.

The workflow shown in :numref:`workflow` is driven by a :mathfunc:`driver(Spill)` node that is configured to process all the `Spill`\ s in the specified HDF5 input files.
The :mathfunc:`driver(Spill)` communicates with the IO system to determine what `Spill`\ s are available for processing.
All driver algorithms know about the single `Job`\ -level data cell that corresponds to the entirety of the framework program execution (the job).
The driver emits the single `Job` object, and a family of `Spill` objects, which cause the *provide* algorithms to create their products, thus starting the data flow through the graph.

Other driver algorithms support different processing workflows.
In one example workflow, a single `Spill` could be reconstructed multiple times with different calibration objects used in the reconstruction, thus allowing a calibration study for a single spill to be performed in one framework execution.
In another example a different driver algorithm could be used to drive the framework to process the Cartesian product of a set of `Spill`\ s and a set of `Calibration` data cells, to perform a different calibration study.
