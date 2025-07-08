=================
Function Notation
=================

The expression :math:`f: A \rightarrow B` represents the function :math:`f` that takes an element of the set :math:`A` to an element of the set :math:`B`.
For example, the function :math:`flip: \mathbb{R} \rightarrow \mathbb{R}` accepts a real number (e.g. :math:`3.14`) and multiplies it by :math:`-1`, returning another real number (e.g. :math:`-3.14`).

The types :math:`A` and :math:`B` are allowed to represent Cartesian products of sets (e.g. :math:`A = A_1 \times \dots \times A_n`), thus enabling multivariate functions.
For example, the following are simple multivariate functions:

.. math::

    power : \mathbb{R} \times \mathbb{N} \rightarrow \mathbb{R}

    halve : \mathbb{N} \rightarrow \mathbb{N} \times \mathbb{N}

where :math:`\mathbb{N}` is the set of natural numbers.
Invoking :math:`power(1.5, 2)` results in the real number :math:`1.5^2 = 2.25`, whereas invoking :math:`halve(5)` divides :math:`5` by :math:`2`, returning the pair :math:`(2, 1)`, where the first number is the quotient, and the second is the remainder.

It is conventional to use parentheses to denote the application of the function :math:`f` to an argument :math:`x` ---i.e. :math:`f(x)`.
To avoid cluttered expressions in this document, however, we adopt an alternative whereby the parentheses are omitted and whitespace is used to delimit the function and its argument(s):

.. math::

   f(x) &\Longrightarrow f\ x

   h(g(y)) &\Longrightarrow h\ (g\ y) = h\ g\ y

In mathematical expressions, parentheses are then reserved in this document to specify:

1. A tuple of arguments that serve as input to a single function invocation (e.g. :math:`power\ (1.5, 2)` above) [#apply]_.
2. The binding of user-defined algorithms to higher-order functions (see :numref:`ch_preliminaries/functional_programming:families of data and higher-order functions`).

.. rubric:: Footnotes

.. [#apply] The expression :math:`f\ x` can equivalently be written :math:`f\ (x)`, where :math:`(x)` is a single-element tuple.
            With this interpretation, the conventional function-invocation notation can be recovered.
