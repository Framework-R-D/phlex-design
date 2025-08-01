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
- naturally reproducible results, assuming the functions are pure (see :numref:`ch_preliminaries/functional_programming:Pure functions`),
- parallel invocations of pure functions with no possibility of data races :need:`DUNE 130`.

Pure Functions
--------------

According to Wikipedia [Wiki-Pure]_, a pure function has the following properties:

- the function return values are identical for identical arguments, and
- the function has no side effects.

Phlex therefore encourages the use of pure functions for creating of :term:`reproducible` data products, a principle of the framework philosophy as discussed in :numref:`introduction:Framework philosophy`.

.. admonition:: Favor free functions

    Functions can additionally be classified as *free functions* or *member functions* (or *methods*).
    Whereas a free function has only explicit input parameters, a member function called on an object has access to the internal state of the object as well as the explicit function parameters.
    Both kinds of functions can be useful, but authors of classes must exercise special care to ensure that a class instance's member functions can be safely invoked in concurrent contexts.
    For this reason, framework users should favor free functions over classes and their member functions.

Challenges with Functional Programming
--------------------------------------

One drawback to functional programming is that it differs from what many in the HEP community are accustomed to when writing their own physics algorithms.
Commonly used third-party libraries and computing languages can also make functional programming difficult to use in practice.
We argue, though, that physicists often think in terms of functional programming when developing the high-level processing steps of a workflow.
It is not until those processing steps need to be implemented that the functional steps are often translated into *procedural* ones.

Phlex aims to restore the functional programming approach as the natural way of expressing the data-processing to be performed.
By leveraging commonly used processing patterns (see :numref:`ch_preliminaries/functional_programming:Families of Data and higher-order functions` on higher-order functions), we can mitigate any awkwardness due to initial unfamiliarity with functional programming paradigms.
The framework also does not place constraints on the algorithm *implementations*---algorithm authors are free to employ imperative programming techniques within the implementations if doing so is convenient.
The framework will simply schedule the algorithm as if it were a pure function without regard to its implementation.

Families of Data and Higher-Order Functions
============================================

Particle physics results are obtained by performing statistical analysis on families of data.
Such analysis typically involves repeated invocations of the same kind of operation.
For example, a relatively simple result is calculating the arithmetic mean of :math:`n` numbers:

.. math::
   \overline{c} = \frac{1}{n}\sum_{i \in \mathcal{I}} c_i

where the sum is over the numbers :math:`\family{c}`, and :math:`n` is the cardinality of the index set :math:`\mathcal{I}`.

The sum is an example of a data reduction or *fold*, where a family is collapsed into one result.
In particular, the arithmetic mean above can be expressed as:

.. math::
   \overline{c} = \frac{1}{n}\ \fold{+}{0}\ \family{c}

where the fold accepts a binary operator (:math:`+` in this case) that is repeatedly applied to an accumulated value (initialized to 0) and each element of the family.

The fold is an example of a *higher-order function* (HOF) [Wiki-HOF]_ that accepts a family and an operator applied in some way to elements of that family.
However, additional HOFs exist---for example, suppose the family :math:`\fami{c}` was created by applying a function :math:`w: E \rightarrow C` to each element of a family :math:`\fami{e}`.
Such a HOF is called a map or *transform*:

.. math::
   \fami{c} = \fami{w\ e} = \transform{w}\ \fami{e}

In such a scenario, the average :math:`\overline{c}` could be expressed as:

.. math::
   \overline{c} = \frac{1}{n}\ \fold{+}{0}\ \transform{w}\ \fami{e} = \frac{1}{n}\ \fold{+ \comp w}{0}\ \fami{e}

The second equality holds by the fold-map fusion law [Bird]_, which states that the application of a :math:`\text{transform}` followed by a :math:`\text{fold}` can be reduced to a single :math:`\text{fold}`.
The operator to this single fold is ':math:`+ \comp w`', indicating that the function :math:`w` should be applied first before invoking the :math:`+` operation.
Relying on such mathematical laws permits the replacement of chained calculations with a single calculation, often leading to efficiency improvements without affecting the result.

.. _hofs_in_phlex:

.. table:: Higher-order functions supported by Phlex.
           Each family is represented by a single variable (e.g. :math:`a`).
           Details of each HOF and its operators are in :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order functions`.
   :widths: 15 30 30 25

   +----------------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------+---------------------------+
   | **Higher-order function**                                                                                                  | Operator(s)                                                 | Output family length      |
   +===================================================================================+========================================+=============================================================+===========================+
   | :ref:`Transform <ch_conceptual_design/hofs/transforms:Transforms>`                | :math:`b = \transform{f}\ a`           | :math:`f: A \rightarrow B`                                  | :math:`|b| = |a|`         |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Predicate <ch_conceptual_design/hofs/predicates:Predicates>`                | :math:`\tilde{b} = \predicate{f}\ a`   | :math:`f: A \rightarrow \bool`                              | :math:`|\tilde{b}| = |a|` |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Filter <ch_conceptual_design/hofs/filters:Filtering>`                       | :math:`a' = \filter{\phi}\ a`          | :math:`\phi: \bool^n \rightarrow \bool`                     | :math:`|a'| \le |a|`      |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Observer <ch_conceptual_design/hofs/observers:Observers>`                   | :math:`[\ \ ] = \observe{f}\ a`        | :math:`f: A \rightarrow \one`                               | :math:`0`                 |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Fold <ch_conceptual_design/hofs/partitioned_folds:Partitioned Folds>`       | :math:`d = \pfold{f}{init}{part}\ c`   | :math:`f: D \times C \rightarrow D`                         | :math:`|d| \le |c|`       |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`init: \opt{\iset{d}} \rightarrow D`                  |                           |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`part: \{\iset{c}\} \rightarrow \mathbb{P}(\iset{c})` |                           |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Unfold <ch_conceptual_design/hofs/partitioned_unfolds:Partitioned Unfolds>` | :math:`c = \punfold{p}{gen}{label}\ d` | :math:`p: N \rightarrow \bool`                              | :math:`|c| \ge |d|`       |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`gen: N \rightarrow N \times C`                       |                           |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`label: \one \rightarrow L`                           |                           |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+
   | :ref:`Window <ch_conceptual_design/hofs/windows:Windows>`                         | :math:`y = \window{f}{adj}{label}\ x`  | :math:`f: X \times \opt{X} \rightarrow Y`                   | :math:`|y| = |x|`         |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`adj: \iset{x} \times \iset{x} \rightarrow \bool`     |                           |
   |                                                                                   |                                        +-------------------------------------------------------------+                           |
   |                                                                                   |                                        | :math:`label: \one \rightarrow L`                           |                           |
   +-----------------------------------------------------------------------------------+----------------------------------------+-------------------------------------------------------------+---------------------------+

A calculation using a HOF is then generally expressed in terms of:

1. The HOF to be used (:math:`\text{fold}`, :math:`\text{transform}`, etc.)
2. The operator(s) to be used by each HOF (:math:`+`, :math:`w`, etc.)
3. The family (or families) of data on which the HOFs are to be applied.

.. index:: Algorithm

Phlex supports the HOFs listed in :numref:`hofs_in_phlex`.
As discussed later, each HOF's *operator* is an :term:`algorithm` registered with the framework.
Phlex will likely support other higher order functions as well.

.. rubric:: Footnotes

.. [#graph] As discussed in :numref:`ch_preliminaries/data_flow:Data flow`, the general topology of a data-processing workflow is a *graph* of functions.

.. only:: html

   .. rubric:: References

.. [Wiki-Pure] https://en.wikipedia.org/wiki/Pure_function
.. [Wiki-HOF] https://en.wikipedia.org/wiki/Higher-order_function
.. [Bird] R. Bird, Introduction to Functional Programming using Haskell (2nd ed.), Prentice Hall (1988), pp. 131–132
