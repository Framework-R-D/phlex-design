Framework Driver
================

In the imperative programming paradigm, control flow structures (such as loops) are provided by the language.
Users of the language are not able to create new control flow facilities.
Previous frameworks contained framework-defined *event loops* that provided the control flow for a framework program.
In the functional programming paradigm, control flow is provided by higher-order functions.
In Phlex, a framework *driver* is a higher-order function that provides the control flow for a framework program.
Phlex provides a variety of framework drivers, and allows users to register their own drivers with the framework, to support different processing needs, e.g. :need:`DUNE 21` and :need:`DUNE 40`.

The workflow shown in :numref:`workflow` is driven by a *SpillDriver* that is configured to process all the *spills* in the specified ROOT input files.
The *SpillDriver* communicates with the IO system to to determine what *spills* are available for processing.
It consults with the graph to ...



.. code:: c++

   PHLEX_REGISTER_DRIVER(config)
   {
   }
