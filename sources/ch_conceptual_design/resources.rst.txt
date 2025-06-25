Resources
=========

As mentioned in :numref:`ch_conceptual_design/algorithms:Algorithms`, the typical form of a C++ function that can be registered with the framework is:

.. code:: c++

   return_type function_name(P1, Pn..., Rm...) [qualifiers];

where the type :cpp:`P1, Pn...` represent data products, and the types :cpp:`Rm...` represent resources.

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

.. todo::

   Refer to :need:`DUNE 36`

User-defined Resources
----------------------

.. todo::

   Refer to :need:`DUNE 149`, :need:`DUNE 152`

.. code:: c++

   PHLEX_REGISTER_RESOURCE(config)
   {
   }
