----------
Transforms
----------

+------------------------------+----------------------------+----------------------+
| **Transform**                | Operator                   | Output family length |
+==============================+============================+======================+
| :math:`b = \transform{f}\ a` | :math:`f: A \rightarrow B` | :math:`|b| = |a|`    |
+------------------------------+----------------------------+----------------------+

The transform is the simplest HOF whose algorithms create data products.
Specifically, the algorithm :math:`f` is applied to each element of the input family :math:`a`, creating a corresponding data product in the output family :math:`b`:

.. math::

   b = \ifamily{b}{a} = \ifamily{f\ a}{a} = \transform{f}\ \ifamily{a}{a}

where :math:`b_i = f\ a_i`.
Note that the index set of the output family is the same as the index set of the input family.

Operator Signature
^^^^^^^^^^^^^^^^^^

.. table::
    :widths: 15 85

    +--------------+-------------------------------------------------------------+
    | **Operator** | **Allowed signature**                                       |
    +==============+=============================================================+
    | :math:`f`    | :cpp:`return_type function_name(P1, Pn..., Rm...) [quals];` |
    +--------------+-------------------------------------------------------------+

The :cpp:`return_type` must model the created data-product type described in :numref:`ch_conceptual_design/algorithms:Return Types`.
An algorithm may also create multiple data products by returning a :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a created data-product type.

Registration Interface
^^^^^^^^^^^^^^^^^^^^^^

To illustrate the different ways a transform's algorithm can be registered with Phlex, we use the following classes and functions, which are presumably defined in some experiment libraries.

.. code:: c++

   class geometry { ... };
   class hits { ... };
   class tracks { ... };
   class vertices { ... };
   class waveforms { ... };

   hits find_hits(waveforms const&) { ... }
   vertices make_vertices(geometry const&, tracks const&) { ... }

   std::tuple<int, int> count_good_hits(hits const&) { ... }
   // Return type: first number = number of good hits
   //              second number = number of all hits

**Transform with one argument (default output product name)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     transform("hit_finder", find_hits, concurrency::unlimited)
       .family("Waveforms"_in("APA"));
   }

**Transform with one argument (user-specified output product name)**

*As shown in* :numref:`workflow` *and described in* :numref:`ch_conceptual_design/registration:Framework Registration`

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("GoodHits") =
       transform("hit_finder", find_hits, concurrency::unlimited)
       .family("Waveforms"_in("APA"));
   }

**Transform with two arguments (default output product name)**

*As shown in* :numref:`workflow` *and described in* :numref:`ch_conceptual_design/registration:Data Products from Different Data Layers`

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("Vertices") =
       transform("vertex_maker", make_vertices, concurrency::unlimited)
       .family("Geometry"_in("Job"), "GoodTracks"_in("APA"));
   }

**Transform creating two data products (user-specified output product names)**

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("NumGoodHits", "NumAllHits") =  // <= One name per tuple slot of return type
       transform("hit_counter", count_good_hits, concurrency::unlimited)
       .family("GoodHits"_in("APA"));
   }
