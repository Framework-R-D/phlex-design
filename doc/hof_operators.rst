HOF operators
=============

Transforms
----------

+------------------------------+----------------------------+------------------------+
| **Transform**                | Operator                   | Output sequence length |
+==============================+============================+========================+
| :math:`b = \transform{f}\ a` | :math:`f: A \rightarrow B` | :math:`|b| = |a|`      |
+------------------------------+----------------------------+------------------------+

**Return type**: A transform algorithm may create multiple data products by returning an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

Filters and predicates
----------------------

+----------------------------+-----------------------------------------+------------------------+
| **Filter**                 | Operator (predicate)                    | Output sequence length |
+============================+=========================================+========================+
| :math:`a' = \filter{p}\ a` | :math:`p: A \rightarrow \text{Boolean}` | :math:`|a'| \le |a|`   |
+----------------------------+-----------------------------------------+------------------------+

Any user-defined algorithm or output sink may be configured to operate on data that satisfy a Boolean condition or *predicate*.
The act of restricting the invocation of a function to data that satisfy a predicate is known as *filtering*.
To filter data as presented to a given algorithm, one or more predicates must be specified in a *filter clause*.
Phlex will not schedule a predicate for execution if it is not bound to a filter.

.. todo::

   Define filter clause.
   Many algorithms can specify the same predicate in their filter clauses without executing the predicate multiple times.

Phlex will only schedule a filter for execution if there is at least one non-filter algorithm or output sink downstream of it.
Predicates can be evaluated on (e.g.) run-level data-product sets and applied to algorithms that process data from data-product sets that are subsets of the run (e.g. events).

Observers
---------

+---------------------------------+--------------------------------------+------------------------+
| **Observer**                    | Operator                             | Output sequence length |
+=================================+======================================+========================+
| :math:`[\ \ ] = \observe{f}\ a` | :math:`f: A \rightarrow \mathbbm{1}` | :math:`0`              |
+---------------------------------+--------------------------------------+------------------------+

As mentioned in :numref:`functional_programming:Higher-order functions supported by Phlex`, observers are a special case of filters that always reject the data presented to them.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Folds
-----

+-------------------------------------+--------------------------------------------------------------+------------------------+
| **Fold**                            | Operators                                                    | Output sequence length |
+=====================================+==============================================================+========================+
| :math:`d = \fold{f}{init}{part}\ c` | :math:`f: D \times C \rightarrow D`                          | :math:`|d| \le |c|`    |
|                                     +--------------------------------------------------------------+                        |
|                                     | :math:`init: \mathbbm{1} \rightarrow D`                      |                        |
|                                     +--------------------------------------------------------------+                        |
|                                     | :math:`part: \{\iset{c}\} \rightarrow \mathcal{P}(\iset{c})` |                        |
+-------------------------------------+--------------------------------------------------------------+------------------------+

Unfolds
-------

+-------------------------------------+-----------------------------------------+------------------------+
| **Unfold**                          | Operators                               | Output sequence length |
+=====================================+=========================================+========================+
| :math:`c = \unfold{p}{gen}{cat}\ d` | :math:`p: D \rightarrow \mbox{Boolean}` | :math:`|c| \ge |d|`    |
|                                     +-----------------------------------------+                        |
|                                     | :math:`gen: D \rightarrow D \times C`   |                        |
|                                     +-----------------------------------------+                        |
|                                     | :math:`cat: \mathbbm{1} \rightarrow L`  |                        |
+-------------------------------------+-----------------------------------------+------------------------+

Unfolds are the opposite of folds, where the output sequence is larger than the input sequence :dune:`17 Unfolding data products`.
An unfold can be used for parallelizing the processing of a data product in smaller chunks.

.. todo:: Explain predicate unfolds here.

Composite CHOFs
---------------
