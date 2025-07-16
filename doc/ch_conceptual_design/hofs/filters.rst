---------
Filtering
---------

+-------------------------------+-----------------------------------------+----------------------+
| **Filter**                    | Operator                                | Output family length |
+===============================+=========================================+======================+
| :math:`a' = \filter{\phi}\ a` | :math:`\phi: \bool^n \rightarrow \bool` | :math:`|a'| \le |a|` |
+-------------------------------+-----------------------------------------+----------------------+

As mentioned in :numref:`ch_conceptual_design/hofs/predicates:Predicates`, the execution of workflow graph can be short-circuited if data products do not meet specified criteria.
This process, known as *filtering*, effectively shortens the input family :math:`a` by retaining only those entries that satisfy a predicate :math:`\phi`, thus creating a different family :math:`a'` composed of elements from :math:`a`.

Filtering is of interest only when there is a downstream node that can receive the filtered family.
Therefore, Phlex will not schedule a filter for execution if the only nodes downstream of it are other filters or predicates.

Filtering can be applied to the input data-product families of any HOFs without explicitly registering a filter HOF.
This is done through a *predicate expression*, which is a stringized form of the predicate :math:`\phi` that is applied to the input data-product family, retaining only the elements that satisfy the predicate.

Predicate Expression
^^^^^^^^^^^^^^^^^^^^

The predicate :math:`\phi` is Boolean expression whose input arguments correspond to the Boolean results of *explicitly registered* predicates.
For example, in :numref:`workflow`, the predicate in :mathfunc:`filter(high_energy)` is :mathfunc:`high_energy`, which is an explicitly registered predicate, as presented in :numref:`ch_conceptual_design/hofs/predicates:Registration interface`.

It is possible for one filter named :cpp:`"only_high_energy"` to use as its predicate :mathfunc:`high_energy`, whereas another filter named :cpp:`"not_high_energy"` could use the negation :math:`\textit{filter}(\neg \textit{high\_energy})`.
In this case, the predicate :mathfunc:`high_energy` is executed only once, but its value can be used in different ways in the predicate expression.

A predicate expression can be evaluated on a higher-level data cell than the data-product family in question.
For example, suppose none of the :product:`GoodHits` data products in a given `Spill` were suitable for processing.
It is possible to create a filter that would reject all :product:`GoodHits` data products from that `Spill` even though the predicate itself interrogated only the `Spill` information and not the lower-level good-hits information from the `APA`.

The supported grammar of the predicate expression is discussed in :numref:`ch_subsystem_design/task_management:Task Management`.

Registration interface
^^^^^^^^^^^^^^^^^^^^^^

The following example shows the complete registration for histogramming the filtered :product:`GoodHits` data products shown in :numref:`workflow`.

.. code:: c++

   class hits { ... };
   void histogram_hits(hits const&, TH1F&) { ... }

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto h_resource = m.resource<histogramming>();

     observe(histogram_hits, concurrency::serial)
       .family("GoodHits"_in("APA"), h_resource->make<TH1F>(...))
       .when("high_energy");  // <= predicate expression within the when(...) call
   }

In practice, it is convenient to specify a predicate expression as part of a Phlex program's run-time configuration instead of hard-coded into a compiled library.
Phlex allows users to specify predicate expressions in a program configuration:

.. code:: jsonnet

   {
     # Speculative configurations for two observer nodes that use the same module library
     histogram_high_energy_hits: {
       plugin: "histogram_hits.so",
     },
     histogram_low_energy_hits: {
       plugin: "histogram_hits.so",
       when: "!high_energy"  # Negate filter, overriding compiled when(...) clause
     },
   }

Note that specifying a predicate expression via the :cpp:`when` configuration parameter overrides whatever predicate expression may have been hard-coded into the compiled module.
