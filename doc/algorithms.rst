Algorithms
==========

For each algorithm, it is possible to specify which hardware resources required by it :dune:`4 Algorithm hardware requirements`.
Specifically, the registration code for an algorithm allows the specification of the maximum number of CPU threads the framework can use when invoking the algorithm :dune:`4.1 Algorithms can use multiple CPUs`.

Configured higher-order functions (CHOFs)
-----------------------------------------

HOF operators
-------------

Transforms
^^^^^^^^^^

+----------------------------+--------------------------------------------------+-----------------------------------------------+
| Transform                                                                     | Resulting sequence                            |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                   | Sequence transformation                          | Dimension                 | Length            |
+============================+==================================================+===========================+===================+
| :math:`f: A \rightarrow B` | .. math::                                        | :math:`\dim(b) = \dim(a)` | :math:`|b| = |a|` |
|                            |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                           |                   |
|                            |    \underbrace{(b_{i_1\dots i_n})}_b             |                           |                   |
+----------------------------+--------------------------------------------------+---------------------------+-------------------+

Filters and predicates
^^^^^^^^^^^^^^^^^^^^^^

+--------------------------------------------------------------------------------------------+---------------------------------------------------+
| Filter                                                                                     | Resulting sequence                                |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| Operator (predicate)                    | Sequence transformation                          | Dimension                  | Length               |
+=========================================+==================================================+============================+======================+
| :math:`p: A \rightarrow \mbox{Boolean}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| \le |a|` |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                         |    \underbrace{(a_{i_1\dots i_n})}_{a'}          |                            |                      |
+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+

Phlex will not schedule a predicate for execution if it is not bound to a filter.
Phlex will also not schedule a filter for execution if there are no non-filter algorithms downstream of it.

Observers
^^^^^^^^^

+----------------------------------------------------------------------------------------+-----------------------------------------------+
| Observer                                                                               | Resulting sequence                            |
+-------------------------------------+--------------------------------------------------+----------------------------+------------------+
| Operator                            | Sequence transformation                          | Dimension                  | Length           |
+=====================================+==================================================+============================+==================+
| :math:`p: A \rightarrow \mathbb{1}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| = 0` |
|                                     |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                  |
|                                     |    \underbrace{(\quad)}_{a'}                     |                            |                  |
+-------------------------------------+--------------------------------------------------+----------------------------+------------------+

As :ref:`mentioned earlier <programming_paradigm:Higher-order functions supported by Phlex>`, observers are a special case of filters that always reject the data presented to them.
Because of this, in a purely functional approach, it is unnecessary to invoke an observer as no data will be produced by an observer.
Additionally, any algorithms downstream of an always-rejecting filter will never be invoked.

However, there are cases where a user may wish to inspect a data product without adjusting the data flow of the program.
This is done by creating an algorithm called an *observer*, which may access a data product but create no data products.
An example of this is writing ROOT histograms or trees that are not intended to be used in another framework program.

Unlike filters and predicates, observers (by definition) are allowed to be the most downstream algorithms of the graph.

Folds
^^^^^

+----------------------------------------------------------------------------------------+-----------------------------------------------+
| Fold                                                                                   | Resulting sequence                            |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operator                            | Sequence transformation                          | Dimension                 | Length            |
+=====================================+==================================================+===========================+===================+
| :math:`g: C \times D \rightarrow D` | .. math::                                        | :math:`\dim(d) < \dim(c)` | :math:`|d| < |c|` |
|                                     |    \underbrace{(c_{i_1\dots i_n})}_c \rightarrow |                           |                   |
|                                     |    \underbrace{(d_{i_1\dots i_m})}_d             |                           |                   |
+-------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Unfolds
^^^^^^^

+--------------------------------------------------------------------------------------------+-----------------------------------------------+
| Unfold                                                                                     | Resulting sequence                            |
+-----------------------------------------+--------------------------------------------------+---------------------------+-------------------+
| Operators                               | Sequence transformation                          | Dimension                 | Length            |
+=========================================+==================================================+===========================+===================+
| .. math::                               | .. math::                                        | :math:`\dim(c) > \dim(d)` | :math:`|c| > |d|` |
|    p: D &\rightarrow& \mbox{Boolean} \\ |    \underbrace{(d_{i_1\dots i_m})}_d \rightarrow |                           |                   |
|    q: D &\rightarrow& D \times C        |    \underbrace{(c_{i_1\dots i_n})}_c             |                           |                   |
+-----------------------------------------+--------------------------------------------------+---------------------------+-------------------+

Composite CHOFs
---------------

Scheduling
----------
