Resources
=========

As mentioned in :numref:`ch_conceptual_design/algorithms:Algorithms`, the typical form of a C++ function that can be registered with the framework is:

.. code:: c++

   return_type function_name(P1, Pn..., Rm...) [qualifiers];

where the types :cpp:`P1, Pn...` represent data products, and the types :cpp:`Rm...` represent resources.

Phlex calls *resources* the object that can be used by an algorithm to let the framework know that the algorithm requires access to a shared entity which is not semantically related to the data-product set hierarchy.
Note that in the requirements that the word "resource" is sometimes used in a more general sense.
In this section we are only referring to the specific kind of resource described above.

An example registration of an algorithm that requires both a data product and a resource is found in :numref:`ch_conceptual_design/hofs/observers:Observers`.
The details of the registration code express to the framework which arguments correspond to data products and which correspond to resources :need:`DUNE 52`.
They may be stateless objects (e.g. a resource that denotes that an algorithm requires the use of a specific thread-unsafe library) or stateful objects (e.g. a resource that denotes access to a GPU, when the platform on which the framework program is running contains several GPUs).
Neither of these examples contain mutable state.
Resources (unlike data products) may have mutable state accessible to the algorithm (e.g. a histogram instance that could be shared across multiple algorithms).
For resources that are mutable, the framework ensures that two algorithms are not interacting with the resource at the same time.
The framework is responsible for efficiently scheduling algorithms based, in part, upon the availability of resources :need:`DUNE 50`.

Examples of resources include:

- GPUs
- Network connections
- Thread-unsafe utilities
- Inference servers
- Database handles :need:`DUNE 35`, :need:`DUNE 40`

Whereas data products have provenance associated with them, resources do not.

Limited Resources
-----------------

.. todo::

   Refer to :need:`DUNE 45`, :need:`DUNE 145`

Explain what a limited resource is and why it is useful.

GPUs
----

.. todo::

   Refer to :need:`DUNE 54`

Say what access to the GPU resource provides (perhaps sole access to the GPU hardware for the time the resource is in scope).

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
