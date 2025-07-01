---------
Filtering
---------

+-------------------------------+-----------------------------------------+------------------------+
| **Filter**                    | Operator                                | Output sequence length |
+===============================+=========================================+========================+
| :math:`a' = \filter{\phi}\ a` | :math:`\phi: \bool^n \rightarrow \bool` | :math:`|a'| \le |a|`   |
+-------------------------------+-----------------------------------------+------------------------+

As mentioned in :numref:`ch_conceptual_design/hofs/predicates:Predicates`, the execution of workflow graph can be short-circuited if data products do not meet specified criteria.
This process, known as *filtering*, effectively shortens the input sequence :math:`a` by retaining only those entries that satisfy a predicate :math:`\phi`, thus creating a different sequence :math:`a'` composed of elements from :math:`a`.

Filtering is of interest only when there is a downstream node that can receive the filtered sequence.
Therefore, Phlex will not schedule a filter for execution if the only nodes downstream of it are other filters or predicates.

Filtering, however, can be applied to the input data-product sequences of any HOFs without explicitly registering a filter HOF.
This is done through a *filter clause*, which is a stringized form of the predicate :math:`\phi` that will be applied to the input data sequence, only retaining elements that satisfy the predicate.

Filter clause
^^^^^^^^^^^^^

The predicate :math:`\phi` is Boolean expression whose arguments correspond to the Boolean results of *explicitly registered* predicates.
For example, in :numref:`workflow`, the predicate in :math:`\textit{filter(high\_energy)}` is :math:`\textit{high\_energy}`, which was an explicitly registered predicate, as presented in :numref:`ch_conceptual_design/hofs/predicates:Registration interface`.

It is possible for one filter named :cpp:`"only_high_energy"` to use as its predicate :math:`\textit{high\_energy}`, whereas another filter named :cpp:`"not_high_energy"` could use the negation :math:`\textit{filter}(\neg \textit{high\_energy})`.
In this case, the predicate :math:`\textit{high\_energy}` is executed only once, but its value can be used in different ways in the filter clause.

Note that it is possible to specify a filter clause that can be evaluated on a higher-level data-product set than the data-product sequence in question.
For example, suppose none of the :cpp:`"GoodHits"` data products in a given `Spill` were suitable for processing.
It is possible to create a filter that would reject all :cpp:`"GoodHits"` data products from that `Spill` even though the predicate itself interrogated only the `Spill` information and not the lower-level good-hits information from the `APA`.

The supported grammar of the filter is discussed in :numref:`ch_subsystem_design/task_management:Task Management`.

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

The following example shows the complete registration for histogramming the filtered :cpp:`"GoodHits"` data products shown in :numref:`workflow`.

.. code:: c++

   class hits { ... };
   void histogram_hits(hits const&, TH1F&) { ... }

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto h_resource = m.resource<histogramming>();

     observe(histogram_hits, concurrency::serial)
       .sequence("GoodHits"_in("APA"), h_resource->make<TH1F>(...))
       .when("high_energy");  // <= filter clause within the when(...) call
   }
