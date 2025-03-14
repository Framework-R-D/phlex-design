Data flow
=========

In the :ref:`description of functional programming <programming_paradigm:Functional programming>`, the example was given for creating tracks from wires.
The simplified expression for doing this was the chained application of two functions :math:`f` and :math:`g` such that:

.. math::
   ts = g(f(ws)) = (g \circ f)(ws)

where :math:`ws` and :math:`ts` are the wires and tracks, respectively.

An alternative representation to the equational form is a *directed graph*

.. graphviz:: graphviz/functional-programming-data-flow.gv
   :align: center

where the functions :math:`f` and :math:`g` are nodes and the data :math:`ws` and :math:`ts` are passed along edges that connect the nodes [#f1]_.
The arrows indicate the flow of the data (or *data flow*) through the graph.

Some observations:

- The function :math:`f` returns hits that are unlabeled in the equational form.
  For the graph above, we explicitly label those hits as :math:`hs` to emphasize that data are always passed along edges.
  If, however, the hits are not needed by any other function in the graph, computational optimizations may be possible (see especially the section below on :ref:`data flow and higher-order functions <data_flow:Optimizations>`).
- A directed graph can have no unterminated edges---i.e. an edge must have a source (the tail of the arrow) and a target (the head of the arrow).
  This means that whereas the equation does not need to specify where the :math:`ws` wires originate (or where the :math:`ts` tracks end up), such locations must be specified in the graph.
  Any node that has only outgoing edges is a *source*, and any node that has only incoming edges is a *sink*.

Data flow and higher-order functions
------------------------------------

As :ref:`mentioned earlier <programming_paradigm:Sequences of data and higher-order functions>`, processing sequences of data is a critical aspect of obtaining physics results.
The data-flow discussion in the previous section naturally maps to applying the functions :math:`f` and :math:`g` to elements of sequences.  Specifically [#f2]_:

- Instead of the individual data objects :math:`ws`, :math:`hs`, and math:`ts` being passed along the edges, the data sequences :math:`(ws_i)`, :math:`(hs_i)`, and :math:`(ts_i)` are passed.
- The functions :math:`f` and :math:`g` map to :math:`\mbox{transform}\left\{f\right\}` and :math:`\mbox{transform}\left\{g\right\}`, respectively.

.. graphviz:: graphviz/functional-programming-data-flow-hof.gv
   :align: center

Optimizations
^^^^^^^^^^^^^

As mentioned above, if a data...

.. graphviz:: graphviz/functional-programming-data-flow-hof-optimized.gv
   :align: center

.. rubric:: Footnotes

.. [#f1] Treating functions as nodes and data as edges is a function-centric view of the graph.
         It is also possible to invert the view of the graph where the data are treated as nodes and the functions as edges; this is the data-centric graph, which is the *line digraph* of the function-centric graph.
.. [#f2] In Haskell (and, correspondingly, in category theory), this transformation is achieved through the `List` functor.
