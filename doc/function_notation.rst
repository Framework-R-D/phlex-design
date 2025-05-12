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

-------------------
Function invocation
-------------------

It is conventional in mathematics to use parentheses to denote the application of the function :math:`f` to an argument :math:`x` ---i.e. :math:`f(x)`.
To avoid cluttered expressions in this document, we adopt an alternative whereby the parentheses are omitted and whitespace is used to delimit the function and its argument(s):

.. math::

   f(x) &\Longrightarrow f\ x\

   g(f(x)) &\Longrightarrow g\ (f\ x) = g\ f\ x \ .

In mathematical expressions, parentheses are then reserved in this document to specify:

1. The order of operations (e.g. :math:`f\ x` is to be evaluated first, and its output serves as input to the function :math:`g`).
2. A tuple of arguments that serve as input to a single function invocation (e.g. :math:`power\ (1.5, 2)` above) [#apply]_.
3. The binding of user-defined algorithms to higher-order functions (see :numref:`functional_programming:Sequences of data and higher-order functions`).

------------------------------------------
Representing :cpp:`void` or :py:`NoneType`
------------------------------------------

In HEP, it is common to encounter C++ functions like:

.. code:: c++

   void f(int);
   double g();

where the function either returns nothing (i.e. :cpp:`void`) or it accepts no argument.
Python supports similar behavior for its functions and methods, but using the keyword :py:`None` instead of :cpp:`void`.

The mathematical set that is used to represent C++'s :cpp:`void` and Python's :py:`NoneType` is the set :math:`\mathbbm{1}`, which contains only one element [#void]_.
The above functions are thus represented in function notation as:

.. math::

   f&: \mathbb{Z} \rightarrow \mathbbm{1}\ , \text{ and}

   g&: \mathbbm{1} \rightarrow \mathbb{R}\ .

This notation will be used as we discuss the operators required by Phlex's higher-order functions.

.. rubric:: Footnotes

.. [#apply] The expression :math:`f\ x` can equivalently be written :math:`f\ (x)`, where :math:`(x)` is a single-element tuple.
            With this interpretation, the conventional function-invocation notation can be recovered.
.. [#void] It is tempting to associate :cpp:`void` and :py:`None` with the empty set :math:`\emptyset`.
           However, a function must always be invoked with at least one object, and it must return at least one object.
           The empty set contains no such objects that can be presented to (or returned from) a function.
