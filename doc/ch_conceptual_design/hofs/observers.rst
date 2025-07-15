Observers
---------

+---------------------------------+--------------------------------------+----------------------+
| **Observer**                    | Operator                             | Output family length |
+=================================+======================================+======================+
| :math:`[\ \ ] = \observe{f}\ a` | :math:`f: A \rightarrow \one`        | :math:`0`            |
+---------------------------------+--------------------------------------+----------------------+

There are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Note that, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by it [#reject]_.
Phlex, however, supports observers as physicists rely on the ability to induce side effects to analyze physics data.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Operator Signature
^^^^^^^^^^^^^^^^^^

.. table::
    :widths: 15 85

    +--------------+------------------------------------------------------+
    | **Operator** | **Allowed signature**                                |
    +==============+======================================================+
    | :math:`f`    | :cpp:`void function_name(P1, Pn..., Rm...) [quals];` |
    +--------------+------------------------------------------------------+

Registration Interface
^^^^^^^^^^^^^^^^^^^^^^

The below shows how the :mathfunc:`histogram_hits` operator in :numref:`workflow` would be registered in C++ [#ffilter]_.
It uses the :cpp:`resource<histogramming>` interface to provide access to a putative histogramming resource (see :numref:`ch_conceptual_design/resources:Resources`).

.. code:: c++

   class hits { ... };
   void histogram_hits(hits const&, TH1F&) { ... }

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto h_resource = m.resource<histogramming>();

     observe(histogram_hits, concurrency::serial)
       .family("GoodHits"_in("APA"), h_resource->make<TH1F>(...));
   }

Note that the number of arguments presented to the :cpp:`family(...)` call matches the number of input parameters of the registered algorithm :cpp:`histogram_hits`.
This indicates that each invocation of :cpp:`histogram_hits` will be presented with one :cpp:`"GoodHits"` data product and the :cpp:`TH1F` resource.

.. rubric:: Footnotes

.. [#reject] An observer is a special case of a filter that rejects all data presented to it.
.. [#ffilter] For this example, we ignore the need to filter :cpp:`"GoodHits"` using the :cpp:`high_energy` predicate.
              This is addressed in :numref:`ch_conceptual_design/hofs/filters:Filtering`.
