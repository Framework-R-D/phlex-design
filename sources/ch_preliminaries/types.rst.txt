=====
Types
=====

A reasonable description of a type is that it is a mathematical *set* of objects.
For example, the type :cpp:`int` is an approximation to the mathematical set :math:`\mathbb{Z}`, although there may be technological limitations on what values an object of type :cpp:`int` can take.

Suppose, however, that an algorithm :math:`p(n_{\text{POT}})` is configured to operate on an integer :math:`n_{\text{POT}}` that corresponds to the number of protons on target.
In such a case, specifying the function :math:`p` as :math:`p: \mathbb{Z} \rightarrow R` (where :math:`R` is an arbitrary return type) is too permissive.
In a framework context, not all data of type :cpp:`int` (the equivalent to :math:`\mathbb{Z}`) are suitable for processing by the algorithm :math:`p`.

In this document, the type therefore refers to a mathematical set that includes more than just the programming language's type :cpp:`T`; it can also include various labels that identify which kind of :cpp:`T` is desired.
When necessary, the type required to specify a data product will be expressed as :math:`\mathtt{T} \wedge L`, where the first set in typewriter style (e.g. :math:`\mathtt{T}`) refers to the programming language's type and the second set in math style (e.g. :math:`L`) represents a label associated with the data.

-------------------------------------------
Representing :cpp:`void` and :py:`NoneType`
-------------------------------------------

In HEP, it is common to encounter C++ functions like:

.. code:: c++

   void f(int);
   double g();

where the function either returns nothing (i.e. :cpp:`void`) or it accepts no argument.
Python supports similar behavior for its functions and methods, but using the keyword :py:`None` instead of :cpp:`void`.

The mathematical set that is used to represent C++'s :cpp:`void` and Python's :py:`NoneType` is the set :math:`\one`, which contains only one element [#void]_.
The above functions are thus represented in function notation as:

.. math::

   f&: \mathbb{Z} \rightarrow \one\ , \text{ and}

   g&: \one \rightarrow \mathbb{R}\ .

This notation will be used as we discuss the operators required by Phlex's higher-order functions.

The single element of the set :math:`\one` can also be used to represent the value :cpp:`nullptr` for C++ pointers (see :numref:`ch_preliminaries/types:Representing optional types`).

---------------------------
Representing optional types
---------------------------

It is occasionally necessary to represent a "nullable" or "optional" type :math:`\text{Opt}(T)`, whose objects either contain a value type :math:`T` or are null.
Mathematically, this is represented by the coproduct :math:`T \sqcup \one`, where a null or *disengaged* object of type :math:`\text{Opt}(T)` has a value equal to the single element of the set :math:`\one`.

:numref:`optional-types` gives examples of programming types in various languages that can be mathematically represented by :math:`\text{Opt}(T)`.
Although Phlex does not support algorithms written in Haskell, an example of the use of :haskell:`Maybe T` is given as an illustration of how :math:`\text{Opt}(T)` is supported outside of C++ and Python.

.. _optional-types:

.. table:: Optional types in Haskell, Python, and C++.
           Due to Python's dynamic nature, a given name (e.g. :py:`k`) can be bound to any value, thus emulating an optional type.
   :widths: 15 10 27 23 25

   +-----------+----------------+------------------------------+-----------------------+--------------------------+
   | Language  | Type :math:`T` | Type :math:`\text{Opt}(T)`   | Engaged value         | Disengaged value         |
   +===========+================+==============================+=======================+==========================+
   | Haskell   | :haskell:`Int` | :haskell:`m :: Maybe Int`    | :haskell:`m = Just 6` | :haskell:`m = Nothing`   |
   +-----------+----------------+------------------------------+-----------------------+--------------------------+
   | Python    | :py:`int`      | *See caption*                | :py:`k = 5`           | :py:`k = None`           |
   +-----------+----------------+------------------------------+-----------------------+--------------------------+
   | C++       | :cpp:`int`     | :cpp:`std::optional<int> i;` | :cpp:`i = 4;`         | :cpp:`i = std::nullopt;` |
   |           |                +------------------------------+-----------------------+--------------------------+
   |           |                | :cpp:`int const* j;`         | :cpp:`j = new int{4};`| :cpp:`j = nullptr;`      |
   +-----------+----------------+------------------------------+-----------------------+--------------------------+

.. rubric:: Footnotes

.. [#void] It is tempting to associate :cpp:`void` and :py:`NoneType` with the empty set :math:`\emptyset`.
           However, a function must always be invoked with at least one object, and it must return at least one object.
           The empty set contains no such objects that can be presented to (or returned from) a function.
