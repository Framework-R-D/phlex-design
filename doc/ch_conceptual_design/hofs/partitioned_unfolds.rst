-------------------
Partitioned Unfolds
-------------------

+----------------------------------------------------------+------------------------------------------------+----------------------+
| **Partitioned unfold**                                   | Operators                                      | Output family length |
+==========================================================+================================================+======================+
| :math:`c = \punfold{p}{\textit{gen}}{\textit{label}}\ d` | :math:`p: N \rightarrow \bool`                 | :math:`|c| \ge |d|`  |
|                                                          +------------------------------------------------+                      |
|                                                          | :math:`\textit{gen}: N \rightarrow N \times C` |                      |
|                                                          +------------------------------------------------+                      |
|                                                          | :math:`\textit{label}: \one \rightarrow L`     |                      |
+----------------------------------------------------------+------------------------------------------------+----------------------+

As discussed in :numref:`ch_preliminaries/functional_programming:Families of Data and Higher-Order Functions`, the opposite of a fold is an *unfold*, where a family of objects is generated from a single object.
The example given in :numref:`ch_preliminaries/data_flow:Data Flow with families` is :math:`\text{iota}`, which generates a sequence of contiguous integers given one input number:

.. math::

    c = [1,\ 2,\ 3,\ \dots,\ n] = \text{iota}\ n = \unfold{greater\_than\_zero}{decrement}\ n

where :math:`\text{iota}` has been expressed in terms of an unfold HOF that receives the predicate :math:`greater\_than\_zero` and a generator called :math:`decrement`.

The unfold operation is repeatedly called until the predicate returns `false`, whereby it emits an empty list :math:`[\ ]`:

.. math::
   :no-wrap:

    \begin{align*}
    c &= \unfold{greater\_than\_zero}{decrement}\ n \\
      &= \unfold{greater\_than\_zero}{decrement}\ n-1\ \boldsymbol{+}\ [n] \\
      &\quad\quad \vdots \\
      &= \unfold{greater\_than\_zero}{decrement}\ 0\ \boldsymbol{+}\ [1, 2, \dots, n-1, n] \\
      &=[\ ]\ \boldsymbol{+}\ [1, 2, \dots, n-1, n]
    \end{align*}

where :math:`\boldsymbol{+}` in this example denotes an operator that concatenates two lists.

Heuristically, this can be thought of as executing the function:

.. code:: python

   def unfold(predicate, generator, n):
       result = []
       next_value = n
       while predicate(next_value):
           # generator returns a new value for next_value
           next_value, family_element = generator(next_value)
           result.prepend(family_element)
       return result

where the user supplies the :py:`predicate` (:math:`p`) and :py:`generator` (:mathfunc:`gen`) algorithms.

Phlex expands the concept of an unfold by allowing it to operate on a family of data products corresponding to a set partition :need:`DUNE 33`.
This *partitioned unfold* is shown in :numref:`workflow`, where the :mathfunc:`unfold(into_apas)` node transforms a flat family of :product:`SimDepos` data products (each of which belong to a cell within the `Spill` partition) into a family of families, with each nested family containing the :product:`Waveforms` data products for all `APA`\ s within a given `Spill`.

Unfolding in this way can be used for parallelizing the processing of a data product in smaller chunks.
Breaking up the processing of a data product can also be an important ingredient in controlling the memory use of a Phlex program.

.. note::

   Phlex requires the use of the :mathfunc:`label` operator in unfolds to avoid collisions with already-existing data products and to reflect the more granular data-processing that occurs as a result of the unfold.

Next Type
^^^^^^^^^

The signatures for the operators :math:`p` and :mathfunc:`gen` have the curious type :math:`N`, which seems unrelated to the input family :math:`d`, whose elements are of type :math:`D`, or the output family :math:`c`, whose elements are of type :math:`C`.
The type :math:`N` refers to the type of the *next* value on which the unfold operates.
In the :math:`\text{iota}` example above, the type :math:`N` is the same as the input argument :math:`n`, which is an integer, and it is the same as that of the output family elements, which are also integers.

The unfold in :numref:`workflow`, however, demonstrates an example where :math:`N` is equal to neither :math:`D` nor :math:`C`.
Whereas the input type :math:`D` corresponds to the :product:`SimDepos` data product in each `Spill`, the output type :math:`C` represents the :product:`Waveforms` data products produced for each `APA`.
Assuming :product:`SimDepos` is represented as a :cpp:`std::vector<SimDepo>` object, a reasonable type for :math:`N` might be :cpp:`std::vector<SimDepo>::const_iterator`, thus permitting the comparison of iterators in the predicate :math:`p` and using it in the generator :mathfunc:`gen` for processing portions of the initial data product.
The generator would thus return a pair with an advanced iterator and a :product:`Waveforms` object corresponding to one `APA`.

The choice of the next type :math:`N` thus depends on the use case and is not prescribed by Phlex.

Operator Signatures
^^^^^^^^^^^^^^^^^^^

.. table::
    :widths: 12 88

    +------------------------+------------------------------------------------------------------------------------+
    | **Operator**           | **Allowed signature**                                                              |
    +========================+====================================================================================+
    | :math:`p`              | :cpp:`bool function_name(next_type) [quals];`                                      |
    +------------------------+------------------------------------------------------------------------------------+
    | :math:`\textit{gen}`   | :cpp:`std::pair<next_type, product_type> function_name(next_type, Rm...) [quals];` |
    +------------------------+------------------------------------------------------------------------------------+
    | :math:`\textit{label}` | *Name of data layer of output data products*                                |
    +------------------------+------------------------------------------------------------------------------------+

The unfold's :cpp:`product_type` must model the created data-product type described in :numref:`ch_conceptual_design/algorithms:Return Types`.
An unfold's :mathfunc:`gen` algorithm may also create multiple data products by returning an object of type :cpp:`std::tuple<next_type, T1, ..., Tn>`, where each of the types :cpp:`T1, ..., Tn` models a created data-product type.

Registration Interface
^^^^^^^^^^^^^^^^^^^^^^

As unfolds require coordination between the predicate :math:`p` and the generator :mathfunc:`gen`, they are supported by implementing classes with member functions that are registered with the framework.

For the :mathfunc:`unfold(to_apas)` node in :numref:`workflow`, the C++ code for the *experiment* algorithm would be:

.. code:: c++

   class sim_depos { ... };
   class waveforms { ... };

   class to_apas {
     using next_type = sim_depos::const_iterator;
     next_type advance(next_type) { ... }
     next_type end_;

   public:
     explicit to_apas(sim_depos const& sds)  // Constructed with input data-product
       : end_{sds.end()}
     {}

     bool keep_going(next_type next) const { return next != end_; }

     std::pair<next_type, waveforms> make_waveforms(next_type next) const
     {
       // Create waveforms object 'ws' using 'next',
       // ... and then move into result
       return std::make_pair(advance(next), std::move(ws));
     }
   };

The definition of :cpp:`advance(...)` would advance the next iterator according to some desired chunk size, or it would return an end iterator when all elements of the :cpp:`"SimDepos"` data product have been processed.
The class is then registered with Phlex via:

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(config)
   {
     products("Waveforms") =
       unfold<to_apas>(
         "to_apas",                 // <= Node name for framework
         &to_apas::keep_going,      // <= Unfold predicate
         &to_apas::make_waveforms,  // <= Unfold generator
         "APA",                     // <= Data layer for output data products
         concurrency::unlimited     // <= Allowed concurrency
       )
       .family("SimDepos"_in("Spill"));
   }

Note that the template argument in :cpp:`unfold<to_apas>` is an indication that the framework will create an object of type :cpp:`to_apas` each time it receives a :cpp:`"SimDepos"` data product.
The framework ensures that all data products remain in memory for as long as they are required, and once they are no longer needed, they (as well as any unneeded :cpp:`to_apas` objects) are evicted from memory as soon as possible :need:`DUNE 142`.
