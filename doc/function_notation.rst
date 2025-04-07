=================
Function notation
=================

We refer to the function :math:`f: A \rightarrow B` as the function :math:`f` that takes an element of the set :math:`A` to an element of the set :math:`B`.
For example, the function :math:`flip: \mathbb{R} \rightarrow \mathbb{R}` accepts a real number (e.g. :math:`3.14`) and multiplies it by :math:`-1`, returning another real number (e.g. :math:`-3.14`).

The types :math:`A` and :math:`B` are allowed to represent Cartesian products of sets (e.g. :math:`A = A_1 \times \dots \times A_n`), thus enabling multivariate functions.
For example, the following are simple multivariate functions:

.. math::

    power : \mathbb{R} \times \mathbb{N} \rightarrow \mathbb{R}

    halve : \mathbb{N} \rightarrow \mathbb{N} \times \mathbb{N}

where :math:`\mathbb{N}` is the set of natural numbers.
Invoking :math:`power(1.5, 2)` results in the real number :math:`1.5^2 = 2.25`, whereas invoking :math:`halve(5)` divides :math:`5` by :math:`2`, returning the pair :math:`(2, 1)`, where the first number is the quotient, and the second is the remainder.
