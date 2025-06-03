Functional Programming
======================

Functional programming is a paradigm that favors the use of *functions* instead of the direct manipulation of stateful objects.
The processing of data happens by using chained operations, where the output of one function serves as the input to other functions.

For example, given two functions:

.. math::
   f: \mbox{Wires} \rightarrow \mbox{Hits}

   g: \mbox{Hits} \rightarrow \mbox{Tracks}

a composite function :math:`h: \mbox{Wires} \rightarrow \mbox{Tracks}` can be constructed such that:

.. math::
   ts = h\ ws = g\ (f\ ws) = (g \comp f)\ ws

or :math:`h = g \comp f`, where :math:`ws \in \mbox{Wires}` and :math:`ts \in \mbox{Tracks}`.

In reality, the creation of tracks from wire signals is much more complicated [#graph]_.
However, as seen above, functional programming permits a mathematical description of the data-processing to be performed.
Expressing the processing needs according to mathematics enables:

- the use of mathematical rules to optimize the processing of the data,
- naturally reproducible results, assuming the functions are pure (see :numref:`functional_programming:Pure functions`),
- parallel invocations of pure functions with no possibility of data races :dune:`58 Thread-safe design for algorithms`.

Pure Functions
--------------

According to Wikipedia [Wiki-pure]_, a pure function has the following properties:

- the function return values are identical for identical arguments, and
- the function has no side effects.

The composition of pure functions is also a pure function.
For example, assuming the functions :math:`f` and :math:`g` above are pure, then the composite function :math:`h` will also be pure.

Using pure functions to create data products ensures :term:`reproducibility <reproducible>`, as discussed in the framework philosophy in :numref:`introduction:Framework philosophy`.

Difficulties with Functional Programming
----------------------------------------

One drawback to functional programming is that it differs from what many in the HEP community are accustomed to when writing their own physics algorithms.
Commonly used third-party libraries and computing languages can also make functional programming difficult to use in practice.
We argue, though, that physicists often think in terms of functional programming when developing the high-level processing steps of a workflow.
It is not until those processing steps need to be implemented that the functional steps are translated into a different programming paradigm (often *procedural*).

Phlex aims to restore the functional programming approach as the natural way of expressing the data-processing to be performed.
By leveraging commonly used processing patterns (see :numref:`functional_programming:Sequences of data and higher-order functions` on higher-order functions), we can mitigate any awkwardness due to initial unfamiliarity with functional programming paradigms.
The framework also does not place constraints on the algorithm *implementations*---algorithm authors are free to employ imperative programming techniques within the implementations if doing so is convenient.
The framework will simply schedule the algorithm as if it were a pure function without regard to its implementation.

Sequences of Data and Higher-Order Functions
============================================

Particle physics results are obtained by performing statistical analysis on sequences of data.
Such analysis typically involves repeated invocations of the same kind of operation.
For example, a relatively simple result is calculating the arithmetic mean of :math:`n` numbers:

.. math::
   \overline{c} = \frac{1}{n}\sum_{i \in \mathcal{I}} c_i

where the sum is over a sequence of numbers :math:`\sequence{c}`, and :math:`n` is the size or *cardinality* of the index set :math:`\mathcal{I}` (e.g. :math:`\{1, 2, \dots, n\}`) used to identify each element of the sequence.

The sum is an example of a data reduction or *fold*, where a sequence is collapsed into one result.
In particular, the arithmetic mean above can be expressed as:

.. math::
   \overline{c} = \frac{1}{n}\ \fold{+}{0}\ \sequence{c}

where the fold accepts a binary operator (:math:`+` in this case) that is repeatedly applied to an accumulated value (initialized to 0) and each element of the sequence.

The fold is an example of a *higher-order function* (HOF) [Wiki-hof]_ that accepts a sequence and an operator applied in some way to elements of that sequence.

Additional HOFs exist---for example, suppose the sequence :math:`[c_i]` was created by applying a function :math:`w: E \rightarrow C` to each element of a sequence :math:`[e_i]`.
Such a HOF is called a map or *transform*:

.. math::
   \sequence{c} = [w\ e_i]_{i \in \mathcal{I}} = \transform{w}\ \sequence{e}

In such a scenario, the average :math:`\overline{c}` could be expressed as:

.. math::
   \overline{c} = \frac{1}{n}\ \fold{+}{0}\ \transform{w}\ \sequence{e} = \frac{1}{n}\ \fold{+ \comp w}{0}\ \sequence{e}

The second equality holds by the fold-map fusion law [Bird]_, which states that the application of a :math:`\text{transform}` followed by a :math:`\text{fold}` can be reduced to a single :math:`\text{fold}`.
The operator to this single fold is ':math:`+ \comp w`', indicating that the function :math:`w` should be applied first before invoking the :math:`+` operation.
Relying on such mathematical laws permits the replacement of chained calculations with a single calculation, often leading to efficiency improvements without affecting the result.

A calculation is then generally expressed in terms of:

1. The HOFs to be used (:math:`\mbox{fold}`, :math:`\mbox{transform}`, etc.)
2. The operation to be used by each HOF (:math:`+`, :math:`w`, etc.)
3. The sequence(s) of data on which the HOFs are to be applied.

Such a formulation lends itself to well-established processing patterns that can be naturally factorized and parallelized.

Higher-Order Functions Supported by Phlex
-----------------------------------------

In general, HOFs transform one sequence to another:

.. math::
    \sequence{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \sequence{a}{\text{input}}

where the functions :math:`f_1, f_2, \ \dots` are *operators* required by the HOF.
Note that the output index set :math:`\iset{\text{output}}` is not necessarily the same as the input index set :math:`\iset{\text{input}}`.

The following table lists the HOFs supported by Phlex.
As discussed later, each HOF's *operator* is an :term:`algorithm` registered with the framework.

.. table::
   :widths: 15 30 30 25

   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+
   | **Higher-order function**                                                                  | Operator(s)                                                 | Output sequence length |
   +======================================================+=====================================+=============================================================+========================+
   | :ref:`Transform <hof_operators:Transforms>`          | :math:`b = \transform{f}\ a`        | :math:`f: A \rightarrow B`                                  | :math:`|b| = |a|`      |
   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+
   | :ref:`Filter <hof_operators:Filters and predicates>` | :math:`a' = \filter{p}\ a`          | :math:`p: A \rightarrow \text{Boolean}`                     | :math:`|a'| \le |a|`   |
   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+
   | :ref:`Observer <hof_operators:Observers>`            | :math:`[\ \ ] = \observe{f}\ a`     | :math:`f: A \rightarrow \mathbbm{1}`                        | :math:`0`              |
   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+
   | :ref:`Fold <hof_operators:Partitioned Folds>`        | :math:`d = \fold{f}{init}{part}\ c` | :math:`f: D \times C \rightarrow D`                         | :math:`|d| \le |c|`    |
   |                                                      |                                     +-------------------------------------------------------------+                        |
   |                                                      |                                     | :math:`init: \mathbbm{1} \rightarrow D`                     |                        |
   |                                                      |                                     +-------------------------------------------------------------+                        |
   |                                                      |                                     | :math:`part: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})` |                        |
   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+
   | :ref:`Unfold <hof_operators:Partitioned Unfolds>`    | :math:`c = \unfold{p}{gen}{cat}\ d` | :math:`p: D \rightarrow \mbox{Boolean}`                     | :math:`|c| \ge |d|`    |
   |                                                      |                                     +-------------------------------------------------------------+                        |
   |                                                      |                                     | :math:`gen: D \rightarrow D \times C`                       |                        |
   |                                                      |                                     +-------------------------------------------------------------+                        |
   |                                                      |                                     | :math:`cat: \mathbbm{1} \rightarrow L`                      |                        |
   +------------------------------------------------------+-------------------------------------+-------------------------------------------------------------+------------------------+

:numref:`registration:HOF operators` describes each of the operators in detail.
Note that the observer is a special case of the filter, where the predicate's Boolean return value is always `false`.

Phlex will likely support other higher order functions as well.

.. rubric:: Footnotes

.. [#graph] As discussed in :numref:`data_flow:Data flow`, the general topology of a data-processing workflow is a *graph* of functions.

.. only:: html

   .. rubric:: References

.. [Wiki-pure] https://en.wikipedia.org/wiki/Pure_function
.. [Wiki-hof] https://en.wikipedia.org/wiki/Higher-order_function
.. [Bird] Bird, Introduction to Functional Programming using Haskell (2nd ed.), Prentice Hall (1988), pp. 131–132
