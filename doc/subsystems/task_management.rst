Task management
^^^^^^^^^^^^^^^

.. todo::
   - Includes building the task graph
   - Includes scheduling of tasks as well as any relevant task priorities or concurrency constraints
   - Respecting data-marshaling demands
   - Relates to algorithms and data (product) sets

The execution of computational tasks will be managed by `oneTBB Flow Graph <https://oneapi-spec.uxlfoundation.org/specifications/oneapi/v1.3-rev-1/elements/onetbb/source/flow_graph>`_.
With oneTBB, the user does not explicitly manage threads but specifies tasks that should be scheduled by oneTBB for execution using its own thread pool.

Depending on the need, oneTBB interface may be explicitly used in algorithm code to take advantage of intra-algorithm parallelism.
Such use will integrate smoothly with Phlex's scheduled tasks as both the framework and the algorithm will be using oneTBB's shared thread pool :dune:`22 Intra-algorithm concurrency and multi-threading`.
