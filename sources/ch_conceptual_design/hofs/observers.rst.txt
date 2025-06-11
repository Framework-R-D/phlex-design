Observers
---------

+---------------------------------+--------------------------------------+------------------------+
| **Observer**                    | Operator                             | Output sequence length |
+=================================+======================================+========================+
| :math:`[\ \ ] = \observe{f}\ a` | :math:`f: A \rightarrow \one`        | :math:`0`              |
+---------------------------------+--------------------------------------+------------------------+

An observer is a special case of a filter that rejects all data presented to it.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

.. table::
    :widths: 15 85

    +--------------+-----------------------------------------------------------+
    | **Operator** | **Allowed signature**                                     |
    +==============+===========================================================+
    | :math:`f`    | :cpp:`void function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+-----------------------------------------------------------+

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

The following classes and functions are presumed to be experiment-defined and are used to demonstrate the registration interface for observers:

.. code:: c++

   class geometry { ... };
   class tracks { ... };
   void check_tracks(tracks const&) { ... }
   void check_vertices(geometry const&, vertices const&) { ... }

**Observer with one argument**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     observe(check_tracks, concurrency::unlimited)
       .sequence("good_tracks"_in("spill"));
   }

**Observer with two arguments**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     observe(check_vertices, concurrency::unlimited)
       .sequence("geometry"_in("job"), "good_tracks"_in("spill"));
   }
