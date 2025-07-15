Resources
=========

As mentioned in :numref:`ch_conceptual_design/algorithms:Algorithms`, the typical form of a C++ function that can be registered with the framework is:

.. code:: c++

   return_type function_name(P1, Pn..., Rm...) [quals];

where the types :cpp:`P1, Pn...` represent data products, and the types :cpp:`Rm...` represent resources.

.. note::
  Phlex calls *resources* the objects that can be used by an algorithm to let the framework know that the algorithm requires access to a shared entity which is not semantically related to the data layer hierarchy.
  Note that in the requirements that the word "resource" is sometimes used in a more general sense.
  In this section we are only referring to the specific kind of resource described above.

An example registration of an algorithm that requires both a data product and a resource is found in :numref:`ch_conceptual_design/hofs/observers:Observers`.
The details of the registration code express to the framework which arguments correspond to data products and which correspond to resources :need:`DUNE 52`.
They may be stateless objects (e.g. a resource that denotes that an algorithm requires the use of a specific thread-unsafe library) or stateful objects (e.g. a resource that denotes access to a GPU, when the platform on which the framework program is running contains several GPUs).
Neither of these examples contain mutable state.
Resources (unlike data products) may have mutable state accessible to the algorithm (e.g. a histogram instance that could be shared across multiple algorithms).
For resources that are thread-unsafe, the framework ensures that two algorithms are not interacting with the resource at the same time.
The framework is responsible for efficiently scheduling algorithms based, in part, upon the availability of resources :need:`DUNE 50`.
To facilitate efficient scheduling of work, the resources needed by the program are specified via configuration or in the algorithm-registration code :need:`DUNE 47` [#fdetails]_.

Examples of resources include:

- GPUs
- Network connections
- Thread-unsafe utilities
- Inference servers
- Database handles :need:`DUNE 35`, :need:`DUNE 40`

Whereas data products have provenance associated with them, resources do not.

Limited Resources
-----------------

Some resources are used to indicate that an algorithm requires sole use of some program entity.
One example of such an entity is a thread-unsafe library, where the framework must ensure that only one algorithm is interacting with that library at any time :need:`DUNE 45`, :need:`DUNE 145`.
A second example of a limited resource is a fixed number of GPUs present on a particular platform, where Phlex must ensure that each algorithm requiring the use of a GPU has sole access to the GPU it is running on for the duration of the algorithm's execution.
A third example of a limited resource could be an algorithm's declaration that it requires spawning some number of threads for its execution (rather than using the framework's task-based execution model).
Such an algorithm could declare the need for the reservation of some number of threads by requiing that number of thread resources :need:`DUNE 152`.
The framework would then ensure that only as many threads as the configuration has provided can be used by algoirthms at any one time.
These resources are called *limited resources*, because the framework is responsible for limiting the access to the resource to one algorithm at a time.

An algorithm to be used by Phlex indicates that it requires a limited resource by requiring an argument that denotes such a resource.

GPUs
----

In order to allow algorithms to make use of GPUs, and to allow the composition of workflows that involve both CPU-based and GPU-based algorithms, Phlex provides a mechanism for an algorithm that requires access to a GPU to declare that fact :need:`DUNE 54`.
This is done by making the algorithm accept a resource that denotes the GPU.
Phlex can support running on platforms that provide access to more than one GPU while ensuring that a given algorithm has sole access to the GPU it requires while it is executing.
Phlex also provides, through the same mechanism, the ability for an algorithm to specify that it requires remote access to a GPU.


Random Number Resource
----------------------

The generation of pseudorandom numbers (hereafter just *random numbers*) is a critical aspect of obtaining physics results, especially when simulating data.
Although multiple random-number generation techniques exists, the vast majority of random-number implementations used in HEP rely on stateful random-number engines, which are subject to data races in multi-threaded contexts.
To ensure reproducible data and to ensure thread-safe access to stateful random-number engines, HEP frameworks impose idiosyncratic constraints on their use.

DUNE has similar requirements on reproducibility of random numbers in a concurrent context :need:`DUNE 36`.
However, instead of working around the limitations of stateful random-number engines, Phlex supports a random-number generation technique specifically designed to reproduce random-numbers in a concurrent program.
Counter-based random number generators (CBRNGs) [Wiki-CBRNG]_ provide such capabilities, and Phlex will provide an interface algorithm authors can use to take advantage of them.

User-defined Resources
----------------------

While Phlex will provide some commonly-used types to represent resources, it will also be possible for users to create new types to represent a new type of resouce, with no modifications to the Phlex framework code :need:`DUNE 149`.
Such resource types have no dependency on Phlex, so that a user algorithm employing such a resource does not thereby incur any dependency on the framework.


.. rubric:: Footnotes

.. [#fdetails] Details for specifying program resources are described in :numref:`ch_subsystem_design/configuration:Program resource specification`.

.. only:: html

   .. rubric:: References

.. [Wiki-CBRNG] https://en.wikipedia.org/wiki/Counter-based_random_number_generator
