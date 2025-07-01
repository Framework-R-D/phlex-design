Resources
=========

As mentioned in :numref:`ch_conceptual_design/algorithms:Algorithms`, the typical form of a C++ function that can be registered with the framework is:

.. code:: c++

   return_type function_name(P1, Pn..., Rm...) [qualifiers];

where the types :cpp:`P1, Pn...` represent data products, and the types :cpp:`Rm...` represent resources.

Resources are used by algorithms that require access to a shared entity that is not semantically related to the data-product set hierarchy.
Examples of resources include:

- GPUs
- Network connections
- Thread-unsafe utilities
- Inference servers
- Database handles

Whereas data products have provenance associated with them, resources do not.

.. todo::

   Refer to :need:`DUNE 35`, :need:`DUNE 40`, :need:`DUNE 50`, :need:`DUNE 52`

.. todo::

   Mention that resources are directly integrated into the framework scheduler.

Limited Resources
-----------------

.. todo::

   Refer to :need:`DUNE 45`, :need:`DUNE 145`

GPUs
----

.. todo::

   Refer to :need:`DUNE 54`

Random Number Resource
----------------------

The generation of pseudorandom numbers (hereafter just *random numbers*) is a critical aspect of obtaining physics results, especially when simulating data.
Although multiple random-number generation techniques exists, the vast majority of random-number implementations used in HEP rely on stateful random-number engines, which are subject to data races in multi-threaded contexts.
To ensure reproducible data and to ensure thread-safe access to stateful random-number engines, HEP frameworks impose idiosyncratic constraints on their use.

DUNE has similar requirements on reproducibility of random numbers in a concurrent context :need:`DUNE 36`.
However, instead of working around the limitations of stateful random-number engines, Phlex supports a random-number generation technique specifically designed to reproduce random-numbers in a concurrent program.
Counter-based random number generators (CBRNGs) [Wiki-CBRNG]_ each contain one internal counter.

User-defined Resources
----------------------

.. todo::

   Refer to :need:`DUNE 149`, :need:`DUNE 152`

.. code:: c++

   PHLEX_REGISTER_RESOURCE(config)
   {
   }

.. only:: html

   .. rubric:: References

.. [Wiki-CBRNG] https://en.wikipedia.org/wiki/Counter-based_random_number_generator
