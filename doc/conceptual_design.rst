Conceptual design
=================

.. admonition:: Purpose

   The conceptual design is not a reference manual; it is a high-level description of how the framework aims to satisfy the :ref:`requirements <requirements:Framework requirements>`.
   The audience for the conceptual design is the physicist, algorithm author, or framework program runner.
   A more technical description of the intended design is given :ref:`here <technical_design:Technical design>`.

Preliminaries
-------------

.. toctree::
   :maxdepth: 2

   programming_paradigm
   data_flow

Public interface
----------------

The framework is a system of integrated aspects that must work together.
To illustrate how these subsystems are designed to work together, we use the below graph to describe each aspect.

.. todo::
   Make graph

.. toctree::
   :maxdepth: 2

   data_organization
   algorithms
   sources_sinks
   resources
   user_configuration
