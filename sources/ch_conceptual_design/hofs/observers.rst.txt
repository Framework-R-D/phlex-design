Observers
---------

+---------------------------------+--------------------------------------+------------------------+
| **Observer**                    | Operator                             | Output sequence length |
+=================================+======================================+========================+
| :math:`[\ \ ] = \observe{f}\ a` | :math:`f: A \rightarrow \one`        | :math:`0`              |
+---------------------------------+--------------------------------------+------------------------+

There are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Note that, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by it [#reject]_.
Phlex, however, supports observers as physicists rely on the ability to induce side effects to analyze physics data.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Operator signature
^^^^^^^^^^^^^^^^^^

.. table::
    :widths: 15 85

    +--------------+-----------------------------------------------------------+
    | **Operator** | **Allowed signature**                                     |
    +==============+===========================================================+
    | :math:`f`    | :cpp:`void function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+-----------------------------------------------------------+

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

The below shows how the :cpp:`histogram_hits` operator in :numref:`workflow` would be registered in C++.
It uses the :cpp:`phlex::resource<histogramming>` interface to provide access to a putative histogramming resource (see :numref:`ch_conceptual_design/resources:Resources`).

.. code:: c++

   class hits { ... };
   void histogram_hits(hits const&) { ... }

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     observe(histogram_hits, concurrency::unlimited)
       .sequence("GoodHits"_in("APA"), phlex::resource<histogramming>());
   }

.. rubric:: Footnotes

.. [#reject] An observer is a special case of a filter that rejects all data presented to it.
