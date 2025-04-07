HOF operators
=============

Transforms
----------

+----------------------------+--------------------------------------------------+-----------------------------------------------+
| **Transform**                                                                 | **Resulting sequence**                        |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                   | Sequence transformation                          | Dimension                 | Length            |
+============================+==================================================+===========================+===================+
| :math:`f: A \rightarrow B` | .. math::                                        | :math:`\dim(b) = \dim(a)` | :math:`|b| = |a|` |
|                            |    :no-wrap:                                     |                           |                   |
|                            |                                                  |                           |                   |
|                            |    \(                                            |                           |                   |
|                            |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                           |                   |
|                            |    \underbrace{(b_{i_1\dots i_n})}_b             |                           |                   |
|                            |    \)                                            |                           |                   |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+

**Return type**: A transform algorithm may create multiple data products by returning an :cpp:`std::tuple<T1, ..., Tn>`  where each of the types :cpp:`T1, ..., Tn` models a data-product created type.

Filters and predicates
----------------------

+--------------------------------------------------------------------------------------------+---------------------------------------------------+
| **Filter**                                                                                 | **Resulting sequence**                            |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| Operator (predicate)                    | Sequence transformation                          | Dimension                  | Length               |
+=========================================+==================================================+============================+======================+
| :math:`p: A \rightarrow \text{Boolean}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| \le |a|` |
|                                         |    :no-wrap:                                     |                            |                      |
|                                         |                                                  |                            |                      |
|                                         |    \(                                            |                            |                      |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_{a'}          |                            |                      |
|                                         |    \)                                            |                            |                      |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+

Phlex will not schedule a predicate for execution if it is not bound to a filter.
Phlex will also not schedule a filter for execution if there are no non-filter algorithms downstream of it.
Predicates can be evaluated on (e.g.) run-level data-product sets and applied to algorithms that process data from data-product sets that are subsets of the run (e.g. events).

.. admonition:: Chris Green
   :class: admonition-chg

   Is it reaasonable to want to use a predicate to filter output?
   This would seem to be in conflict with the "no downstream non-filter algorithms" restriction.

Observers
---------

+-----------------------------------------------------------------------------------------+-----------------------------------------------+
| **Observer**                                                                            | **Resulting sequence**                        |
+--------------------------------------+--------------------------------------------------+----------------------------+------------------+
| Operator                             | Sequence transformation                          | Dimension                  | Length           |
+======================================+==================================================+============================+==================+
| :math:`p: A \rightarrow \mathbbm{1}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| = 0` |
|                                      |    :no-wrap:                                     |                            |                  |
|                                      |                                                  |                            |                  |
|                                      |    \(                                            |                            |                  |
|                                      |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                  |
|                                      |    \underbrace{(\quad)}_{a'}                     |                            |                  |
|                                      |    \)                                            |                            |                  |
+--------------------------------------+--------------------------------------------------+----------------------------+------------------+

As :ref:`mentioned earlier <functional_programming:Higher-order functions supported by Phlex>`, observers are a special case of filters that always reject the data presented to them.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Folds
-----

+----------------------------------------------------------------------------------------+-----------------------------------------------+
| **Fold**                                                                               | **Resulting sequence**                        |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                            | Sequence transformation                          | Dimension                 | Length            |
+=====================================+==================================================+===========================+===================+
| :math:`g: C \times D \rightarrow D` | .. math::                                        | :math:`\dim(d) < \dim(c)` | :math:`|d| < |c|` |
|                                     |    :no-wrap:                                     |                           |                   |
|                                     |                                                  |                           |                   |
|                                     |    \(                                            |                           |                   |
|                                     |    \underbrace{(c_{i_1\dots i_n})}_c \rightarrow |                           |                   |
|                                     |    \underbrace{(d_{i_1\dots i_m})}_d             |                           |                   |
|                                     |    \)                                            |                           |                   |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Unfolds
-------

+--------------------------------------------------------------------------------------------+-----------------------------------------------+
| **Unfold**                                                                                 | **Resulting sequence**                        |
+-----------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operators                               | Sequence transformation                          | Dimension                 | Length            |
+=========================================+==================================================+===========================+===================+
| :math:`p: D \rightarrow \text{Boolean}` | .. math::                                        | :math:`\dim(c) > \dim(d)` | :math:`|c| > |d|` |
|                                         |    :no-wrap:                                     |                           |                   |
+-----------------------------------------+                                                  |                           |                   |
| :math:`q: D \rightarrow D \times C`     |    \(                                            |                           |                   |
|                                         |    \underbrace{(d_{i_1\dots i_m})}_d \rightarrow |                           |                   |
|                                         |    \underbrace{(c_{i_1\dots i_n})}_c             |                           |                   |
|                                         |    \)                                            |                           |                   |
+-----------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Unfolds are the opposite of folds, where the output sequence is larger than the input sequence :dune:`17 Unfolding data products`.
An unfold can be used for parallelizing the processing of a data product in smaller chunks.

.. todo:: Explain predicate unfolds here.

Composite CHOFs
---------------
