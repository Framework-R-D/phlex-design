Functional programming
----------------------

Phlex supports a *functional programming* paradigm, where data-processing occurs by favoring the use of *functions* instead of the direct manipulation of stateful objects.
The processing of data happens by using chained operations, where the output of one function serves as the input to other functions.

For example, given two functions:

.. math::
   f: \mbox{Wires} \rightarrow \mbox{Hits}

   g: \mbox{Hits} \rightarrow \mbox{Tracks}

a composite function :math:`h: \mbox{Wires} \rightarrow \mbox{Tracks}` can be constructed such that:

.. math::
   t = h(w) = g(f(w)) = (f \circ g)(w)

or :math:`h = f \circ g`, where :math:`w \in \mbox{Wires}` and :math:`t \in \mbox{Tracks}`.

In reality, the creation of tracks from wire signals is much more complicated [#f1]_.
However, as seen above, functional programming permits a mathematical description of the data-processing to be performed.
Expressing the processing needs according to mathematics enables:

- the use of mathematical rules to optimize the processing of the data,
- naturally reproducible results, assuming the functions are :ref:`pure <pure-functions>`,
- parallel invocations of pure functions with no possibility of data races.

.. _pure-functions:

Pure functions
^^^^^^^^^^^^^^

According to Wikipedia [Wiki-pure]_, a pure function has the following properties:

- the function return values are identical for identical arguments, and
- the function has no side effects.

The composition of pure functions is also a pure function.
For example, assuming the functions :math:`f` and :math:`g` above are pure, then the composite function :math:`h` will also be pure.

Difficulties with functional programming
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One drawback to functional programming is that it differs from what many in the HEP community are accustomed to when writing their own physics algorithms.
Commonly used third-party libraries and computing languages can also make functional programming difficult to use in practice.
We argue, though, that physicists often think in terms of functional programming when developing a workflow about the high-level processing steps.
It is not until those processing steps need to be implemented that the functional steps are translated into a different programming paradigm (often *procedural*).

Phlex aims to restore the functional programming approach as the natural way of expressing the data-processing to be performed.
By leveraging commonly used processing patterns (see next section on :ref:`higher-order functions <higher-order-functions>`), any awkwardness due to functional programming is largely alleviated.

.. _higher-order-functions:

Sequences of data and higher-order functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Particle physics results are often

Higher-order functions well-defined patterns, including:

* transforms (or maps)
* folds (or reductions)
* unfolds (the opposite of folds)
* predicates (used during filtering)
* observers
* sliding windows

.. _graph-processing:

Graphs of higher-order functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. rubric:: Footnotes

.. [#f1] As discussed :ref:`here <graph-processing>`, the general topology of a data-processing workflow is a *graph* of functions.

.. rubric:: References

.. [Wiki-pure] https://en.wikipedia.org/wiki/Pure_function
.. [Wiki-hof] https://en.wikipedia.org/wiki/Higher-order_function
