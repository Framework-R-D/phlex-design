Data Flow
=========

In :numref:`ch_preliminaries/functional_programming:Functional programming`, the example was given for creating tracks from wires.
The simplified expression for doing this was the chained application of two functions :math:`f` and :math:`g` such that:

.. math::
   ts = g\ (f\ ws) = (g \comp f)\ ws

where :math:`ws` and :math:`ts` are the wires and tracks, respectively.

An alternative representation to the equational form is a *directed graph*

.. graphviz:: functional-programming-data-flow.gv
   :align: center

where the functions :math:`f` and :math:`g` are vertices or *nodes* and the data :math:`ws` and :math:`ts` are passed along arrows or *edges* that connect the nodes [#f1]_.
The arrows indicate the flow of the data (or *data flow*) through the graph.

Some observations:

- The function :math:`f` returns hits that are unlabeled in the equational form.
  For the graph above, we explicitly label those hits as :math:`hs` to emphasize that data are always passed along edges.
  If, however, the hits are not needed by any other function in the graph, the two functions :math:`f` and :math:`g` can be replaced by their composition :math:`h=g \circ f`

  .. graphviz:: functional-programming-data-flow-optimized.gv
     :align: center

  resulting in potential performance improvements in computational efficiency and program memory usage.

- A directed graph can have no unterminated edges---i.e. an edge must have a source (the tail of the arrow) and a target (the head of the arrow).
  This means that whereas the equation does not need to specify where the :math:`ws` wires originate (or where the :math:`ts` tracks end up), such locations must be specified in the graph.
  Any node that has only outgoing edges is called a *source* (denoted by a solid dot), and any node that has only incoming edges is a *sink* (denoted by an encircled solid dot).

Data Flow with Families
------------------------

As mentioned earlier in :numref:`ch_preliminaries/functional_programming:Families of Data and higher-order functions`, processing families of data is a critical aspect of obtaining physics results.
The data-flow discussion in the previous section naturally maps to applying the functions :math:`f` and :math:`g` to elements of families.  Specifically [#f2]_:

- Instead of the individual objects :math:`ws`, :math:`hs`, and :math:`ts` being passed along the edges, the families :math:`\fami{ws}`, :math:`\fami{hs}`, and :math:`\fami{ts}` are passed.
- The functions :math:`f` and :math:`g` map to :math:`\transform{f}` and :math:`\transform{g}`, respectively.

.. graphviz:: functional-programming-data-flow-hof.gv
   :align: center

The above graph does not specify an implementation---assuming :math:`f` and :math:`g` are pure functions (see :numref:`ch_preliminaries/functional_programming:Pure functions`), the same result is obtained if (a) full families are passed between the nodes, or (b) one element per family is passed at a time.
Determining whether option (a) or (b) is more efficient depends on the data passed between the nodes and the overall constraints on the program.

One benefit to using a graph representation using data families and higher-order functions is the ability to easily express folds and unfolds.
As an illustration, consider the calculation of a sum of squares for all integers from 1 through :math:`n`:

.. math::
   S(n) = \sum_{i=1}^n i^2

This calculation requires three separate steps:

1. an unfold called :math:`\textit{iota}` that, given an integer :math:`n`, generates a sequence of integers from 1 through :math:`n`,
2. a transformation that squares each integer in the sequence using an algorithm called :math:`\textit{square}`, and
3. a fold called :math:`\textit{sum}` that sums all squared integers.

The data-flow graph of individual objects looks like:

.. graphviz:: unfold-transform-fold.gv
   :align: center

where each number is passed along its own edge to the nodes performing the calculation.

The summation formula for :math:`S(n)`, however, can be expressed in terms of higher-order functions that reflect the three steps above:

.. math::
   S(n) = \sum_{i=1}^n i^2 = \underbrace{\fold{+}{0}}_{3.\ \text{sum}}\ \underbrace{\transform{\textcolor{blue}{\textit{square}}}}_{2.}\ \underbrace{\unfold{greater\_than\_zero}{decrement}}_{1.\ \text{iota}}\ n

where:

1. :math:`\text{iota}` or :math:`\unfold{greater\_than\_zero}{decrement}` is a function that returns a sequence given a value of :math:`n`,
2. :math:`\transform{\textcolor{blue}{\textit{square}}}` is a function applied to the sequence generated in step 1.
3. :math:`\text{sum}` or :math:`\fold{+}{0}` is a function that returns a single result when applied to the result of step 2.

Note that the unfold takes two operators: the predicate :math:`greater\_than\_zero`, which tests whether the next generated number is greater than zero, and the :math:`decrement` operator, which decrements the next generated number by 1. [#f3]_
Once the predicate returns false, the unfold stops generating numbers.

By adopting the HOF representation of the sum-of-squares problem, the data-flow graph is simplified:

.. graphviz:: unfold-transform-fold-hof.gv
   :align: center

In addition, the topology of the family-based graph remains the same regardless of the value of :math:`n`; the topology of the object-based graph becomes intractable as :math:`n` increases.

The vast majority of graphs included in this design document adopt the family-based data-flow representation.

.. rubric:: Footnotes

.. [#f1] Treating functions as nodes and data as edges is a function-centric view of the graph.
         It is also possible to invert the view of the graph such that the data are treated as nodes and the functions as edges; this is the data-centric graph, which is the *line digraph* of the function-centric graph.
.. [#f2] In Haskell (and, similarly, in category theory), this transformation is achieved through the `List/fmap` functor.
.. [#f3] The same data flow can equivalently be represented with an unfold that uses the predicate :math:`less\_than\_or\_equal\_to\_n` and the operator :math:`\textit{increment}`, but starts with an initial value of :math:`1`.
         This other representation, however, requires an unfold predicate that depends on the value of :math:`n`.
