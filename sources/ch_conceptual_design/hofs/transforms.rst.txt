----------
Transforms
----------

+------------------------------+----------------------------+------------------------+
| **Transform**                | Operator                   | Output sequence length |
+==============================+============================+========================+
| :math:`b = \transform{f}\ a` | :math:`f: A \rightarrow B` | :math:`|b| = |a|`      |
+------------------------------+----------------------------+------------------------+

The transform is the simplest HOF whose algorithms create data products.
Specifically, the algorithm :math:`f` is applied to each element of the input sequence :math:`a`, creating a corresponding data product in the output sequence :math:`b`:

.. math::

   \isequence{b}{a} = \isequence{f\ a}{a} = \transform{f}\ \isequence{a}{a}

where :math:`b_i = f\ a_i`.
Note that the index set of the output sequence is the same as the index set of the input sequence.

.. todo::

   Allow for transforms where the output sequence is indexed by a different set—i.e. the number of elements remains the same as the input sequence, but the *label* of those elements changes.

.. table::
    :widths: 15 85

    +--------------+------------------------------------------------------------------+
    | **Operator** | **Allowed signature**                                            |
    +==============+==================================================================+
    | :math:`f`    | :cpp:`return_type function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+------------------------------------------------------------------+

**Return type**: A transform algorithm may create multiple data products by returning an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

To illustrate the different ways a transform's algorithm can be registered with Phlex, we use the following classes and functions, which are presumably defined in some experiment libraries.

.. code:: c++

   class geometry { ... };
   class hits { ... };
   class tracks { ... };
   class vertices { ... };

   tracks make_tracks(hits const&) { ... }
   vertices make_vertices(geometry const&, tracks const&) { ... }

**Transform with one argument (default output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     transform("track_maker", make_tracks, concurrency::unlimited)
       .sequence("good_hits"_in("spill"));
   }

**Transform with one argument (user-specified output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     products("good_tracks") =
       transform("track_maker", make_tracks, concurrency::unlimited)
       .sequence("good_hits"_in("spill"));
   }

**Transform with two arguments (default output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     transform("vertex_maker", make_vertices, concurrency::unlimited)
       .sequence("geometry"_in("job"), "good_hits"_in("spill"));
   }
