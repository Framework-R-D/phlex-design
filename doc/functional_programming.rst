Functional programming
======================

Functional programming is a paradigm that favors the use of *functions* instead of the direct manipulation of stateful objects.
The processing of data happens by using chained operations, where the output of one function serves as the input to other functions.

For example, given two functions:

.. math::
   f: \mbox{Wires} \rightarrow \mbox{Hits}

   g: \mbox{Hits} \rightarrow \mbox{Tracks}

a composite function :math:`h: \mbox{Wires} \rightarrow \mbox{Tracks}` can be constructed such that:

.. math::
   ts = h(ws) = g(f(ws)) = (g \comp f)(ws)

or :math:`h = g \comp f`, where :math:`ws \in \mbox{Wires}` and :math:`ts \in \mbox{Tracks}`.

In reality, the creation of tracks from wire signals is much more complicated [#f1]_.
However, as seen above, functional programming permits a mathematical description of the data-processing to be performed.
Expressing the processing needs according to mathematics enables:

- the use of mathematical rules to optimize the processing of the data,
- naturally reproducible results, assuming the functions are :ref:`pure <functional_programming:Pure functions>`,
- parallel invocations of pure functions with no possibility of data races :dune:`58 Thread-safe design for algorithms`.

Pure functions
--------------

According to Wikipedia [Wiki-pure]_, a pure function has the following properties:

- the function return values are identical for identical arguments, and
- the function has no side effects.

The composition of pure functions is also a pure function.
For example, assuming the functions :math:`f` and :math:`g` above are pure, then the composite function :math:`h` will also be pure.

Using pure functions to create data products ensures :term:`reproducibility <reproducible>`, as discussed in the :ref:`framework philosophy <introduction:Framework philosophy>`.

Difficulties with functional programming
----------------------------------------

One drawback to functional programming is that it differs from what many in the HEP community are accustomed to when writing their own physics algorithms.
Commonly used third-party libraries and computing languages can also make functional programming difficult to use in practice.
We argue, though, that physicists often think in terms of functional programming when developing a workflow about the high-level processing steps.
It is not until those processing steps need to be implemented that the functional steps are translated into a different programming paradigm (often *procedural*).

Phlex aims to restore the functional programming approach as the natural way of expressing the data-processing to be performed.
By leveraging commonly used processing patterns (see next section on :ref:`higher-order functions <functional_programming:Sequences of data and higher-order functions>`), we can mitigate any awkwardness due to initial unfamiliarity with functional programming paradigms.
The framework also does not place constraints on the algorithm *implementations*---algorithm authors are free to employ imperative programming techniques within the implementations if doing so is convenient.
The framework will simply schedule the algorithm as if it were a pure function without regard to its implementation.

Sequences of data and higher-order functions
============================================

Particle physics results are obtained by performing statistical analysis on sequences of data.
Such analysis typically involves repeated invocations of the same kind of operation.
For example, a relatively simple result is calculating the arithmetic mean of :math:`n` numbers:

.. math::
   \overline{b} = \frac{1}{n}\sum_{i \in \mathcal{I}} b_i

where the sum is over a sequence of numbers :math:`\sequence{b}`, and :math:`n` is the size or *cardinality* of the index set :math:`\mathcal{I}` (e.g. :math:`\{1, 2, \dots, n\}`) used to identify each element of the sequence.

The sum is an example of a data reduction or *fold*, where a sequence is collapsed into one result.
In particular, the arithmetic mean above can be expressed as:

.. math::
   \overline{b} = \mbox{avg} \left\{\sequence{b}\right\} = \frac{1}{n} \fold{+}{0}{\sequence{b}}

where the fold accepts a binary operator (:math:`+` in this case) that is repeatedly applied to an accumulated value (initialized to 0) and each element of the sequence.

The fold is an example of a *higher-order function* (HOF) [Wiki-hof]_ that accepts a sequence and an operator applied in some way to elements of that sequence.

Additional HOFs exist---for example, suppose the sequence :math:`(b_i)` was created by applying a function :math:`p: A \rightarrow B` to each element of a sequence :math:`(a_i)`.
Such a HOF is called a map or *transform*:

.. math::
   \sequence{b} = (p(a_i))_{i \in \mathcal{I}} = \transform{p}{\sequence{a}}

In such a scenario, the average :math:`\overline{b}` could be expressed as:

.. math::
   \overline{b} = \frac{1}{n} \fold{+}{0}{\transform{p}{\sequence{a}}} = \frac{1}{n} \fold{+ \comp p}{0}{\sequence{a}}

The second equality holds by the fold-map fusion law [Bird]_, which states that the application of a :math:`\text{transform}` followed by a :math:`\text{fold}` can be reduced to a single :math:`\text{fold}`.
The operator to this single fold is ':math:`+ \comp p`', indicating that the function :math:`p` should be applied first before invoking the :math:`+` operation.
Relying on such mathematical laws permits the replacement of chained calculations with a single calculation, often leading to efficiency improvements without affecting the result.

A calculation is then generally expressed in terms of:

1. The HOFs to be used (:math:`\mbox{fold}`, :math:`\mbox{transform}`, etc.)
2. The operation to be used by each HOF (:math:`+`, :math:`p`, etc.)
3. The sequence(s) of data on which the HOFs are to be applied.

Such a formulation lends itself to well-established processing patterns that can be naturally factorized and parallelized.

Higher-order functions supported by Phlex
-----------------------------------------

In general, HOFs transform one sequence to another:

.. math::
   (a_{i_1i_2\dots i_n}) \rightarrow (b_{j_1j_2\dots j_m})

where the multiple indices indicate that the sequences can be multidimensional.
Note that :math:`n` is not necessarily equal to :math:`m`.

The following table lists the HOFs supported by Phlex.
Details of each higher-order function are provided by the link to the corresponding section in the User API section.
As discussed later, each HOF's *operator* is an :term:`algorithm` registered with the framework.

+---------------------------------------------------------------------------------------------------------------------------------------------------+---------------------------------------------------+
| **Higher-order function**                                                                                                                         | **Resulting sequence**                            |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| Name                                                 | Operator                                | Sequence transformation                          | Dimension                  | Length               |
+======================================================+=========================================+==================================================+============================+======================+
| :ref:`Transform <hof_operators:Transforms>`          | :math:`f: A \rightarrow B`              | .. math::                                        | :math:`\dim(b) = \dim(a)`  | :math:`|b| = |a|`    |
|                                                      |                                         |    :no-wrap:                                     |                            |                      |
|                                                      |                                         |                                                  |                            |                      |
|                                                      |                                         |    \(                                            |                            |                      |
|                                                      |                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                                      |                                         |    \underbrace{(b_{i_1\dots i_n})}_b             |                            |                      |
|                                                      |                                         |    \)                                            |                            |                      |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| :ref:`Fold <hof_operators:Folds>`                    | :math:`g: C \times D \rightarrow D`     | .. math::                                        | :math:`\dim(d) < \dim(c)`  | :math:`|d| \le |c|`  |
|                                                      |                                         |    :no-wrap:                                     |                            |                      |
|                                                      |                                         |                                                  |                            |                      |
|                                                      |                                         |    \(                                            |                            |                      |
|                                                      |                                         |    \underbrace{(c_{i_1\dots i_n})}_c \rightarrow |                            |                      |
|                                                      |                                         |    \underbrace{(d_{i_1\dots i_m})}_d             |                            |                      |
|                                                      |                                         |    \)                                            |                            |                      |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| :ref:`Unfold <hof_operators:Unfolds>`                | :math:`p: D \rightarrow \mbox{Boolean}` | .. math::                                        | :math:`\dim(c) > \dim(d)`  | :math:`|c| \ge |d|`  |
|                                                      |                                         |    :no-wrap:                                     |                            |                      |
|                                                      +-----------------------------------------+                                                  |                            |                      |
|                                                      | :math:`q: D \rightarrow D \times C`     |    \(                                            |                            |                      |
|                                                      |                                         |    \underbrace{(d_{i_1\dots i_m})}_d \rightarrow |                            |                      |
|                                                      |                                         |    \underbrace{(c_{i_1\dots i_n})}_c             |                            |                      |
|                                                      |                                         |    \)                                            |                            |                      |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| :ref:`Filter <hof_operators:Filters and predicates>` | :math:`p: A \rightarrow \text{Boolean}` | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| \le |a|` |
|                                                      |                                         |    :no-wrap:                                     |                            |                      |
|                                                      |                                         |                                                  |                            |                      |
|                                                      |                                         |    \(                                            |                            |                      |
|                                                      |                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                                      |                                         |    \underbrace{(a_{i_1\dots i_n})}_{a'}          |                            |                      |
|                                                      |                                         |    \)                                            |                            |                      |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+
| :ref:`Observer <hof_operators:Observers>`            | .. math::                               | .. math::                                        | :math:`\dim(a') = \dim(a)` | :math:`|a'| = 0`     |
|                                                      |    :no-wrap:                            |    :no-wrap:                                     |                            |                      |
|                                                      |                                         |                                                  |                            |                      |
|                                                      |    \(p: A \rightarrow \mathbbm{1}\)     |    \(                                            |                            |                      |
|                                                      |                                         |    \underbrace{(a_{i_1\dots i_n})}_a \rightarrow |                            |                      |
|                                                      |                                         |    \underbrace{(\quad)}_{a'}                     |                            |                      |
|                                                      |                                         |    \)                                            |                            |                      |
+------------------------------------------------------+-----------------------------------------+--------------------------------------------------+----------------------------+----------------------+

Note that the observer is a special case of the filter, where the predicate's Boolean return value is always `false`.
The set :math:`\mathbbm{1}` indicates a one-element set whose element can represent a `false` value.
In C++, this would be denoted by the return statement :cpp:`return;` whereas in Python, it would be denoted by either :py:`return` or :py:`return None`.

Phlex will likely support other higher order functions as well, such as a sliding window.

.. admonition:: Chris Green
   :class: admonition-chg

   I'm not sure about the form of the above: if we're going to support a sliding window, then it should have a line in the preceeding table. If we merely wish to state that Phex may be extended straightforwardly to support other higher order functions, then we should say that.

.. rubric:: Footnotes

.. [#f1] As discussed :ref:`here <data_flow:Data flow>`, the general topology of a data-processing workflow is a *graph* of functions.

.. only:: html

   .. rubric:: References

.. [Wiki-pure] https://en.wikipedia.org/wiki/Pure_function
.. [Wiki-hof] https://en.wikipedia.org/wiki/Higher-order_function
.. [Bird] Bird, Introduction to Functional Programming using Haskell (2nd ed.), Prentice Hall (1988), pp. 131–132
