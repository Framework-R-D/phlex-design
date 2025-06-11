Algorithms
==========

As mentioned in :numref:`ch_preliminaries/functional_programming:Sequences of Data and Higher-Order Functions`, an algorithm is registered with the framework as an operator to a higher-order function (HOF).
In general, Phlex supports the registration of C++ algorithms with function signatures like (see :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions` for a list of supported HOFs):

.. code:: c++

   return_type function_name(P1, Pn..., Rm...) [qualifiers];

where the types :cpp:`P1, Pn...` denote types of data products and the types :cpp:`Rm...` indicate :term:`resources <resource>`.
Each registered function must accept at least one data product.

We will first discuss the data-product and resource types in :numref:`ch_conceptual_design/algorithms:Input parameters`, followed by the return types in :numref:`ch_conceptual_design/algorithms:Return types`, and then the function name and optional qualifers in :numref:`ch_conceptual_design/algorithms:Function names and qualifiers`.

Input Parameters
----------------

A data product of type :cpp:`P` may be presented to a C++ algorithm if the corresponding input parameter (i.e. the relevant :cpp:`P1, ..., PN` type) is one of the following:

- :cpp:`P const&` — read-only access to a data product provided through a reference
- :cpp:`P const*` — read-only access to a data product provided through a pointer
- :cpp:`P` — the data product is copied into an object (assumes data product is copyable) [#f1]_
- :cpp:`phlex::handle<P>` — a lightweight object that provides read-only access to a data product as well as any metadata associated with it

For each of these cases, the data product itself remains immutable.

Whereas data products may to be copied, resources of type :cpp:`R` may not.
The following types are therefore supported:

- :cpp:`R const&` — read-only access to a resource provided through a reference
- :cpp:`R const*` — read-only access to a resource provided through a pointer

Resources are described in more detail in :numref:`ch_conceptual_design/resources:Resources`.

Return Types
------------

The meaning of an algorithm's return type depends on the HOF and is discussed in the :numref:`ch_conceptual_design/supported_hofs:Supported Higher-Order Functions`.
However, to simplify the discussion we introduce to concept of the *created data-product type*.
For Phlex to appropriately schedule the execution of algorithms and manage the lifetimes of data products, the framework itself must retain ownership of the data products.
This means that the data products created by algorithms must have types that connote unique ownership.
An algorithm's returned object must therefore model a created data-product type, which can be:

- a *value* of type :cpp:`T`, or
- a :cpp:`std::unique_ptr<T>`, where the created object is non-null.

The following types (or their equivalents) are forbidden as created data-product types because they do not imply unambiguous ownership:

- *bare pointer types*, such as :cpp:`T*` or :cpp:`T const*`
- *reference types*, such as :cpp:`T&` or :cpp:`T const&`

Function Names and Qualifiers
-----------------------------

The :cpp:`function_name` in :numref:`ch_conceptual_design/algorithms:Algorithms` above may be any function name supported by the C++ language.
Code authors should aim to implement algorithms as free functions.
However, in some cases it may be necessary for class member functions to be used instead.
When member functions are required, the qualifier :cpp:`const` should be specified to indicate that the class instance remains immutable during the execution of the member function [#f2]_.

.. rubric:: Footnotes

.. [#f1] In C++, the function signature corresponds to the function *declaration* [CppFunctionDecl]_, for which the type :cpp:`P` and :cpp:`P const` are treated identically by the compiler.
         However, for the function implementation or *definition*, algorithm authors are encouraged to use :cpp:`P const` to help guarantee the immutability of data.
.. [#f2] Phlex permits the registration of member functions that do not use the :cpp:`const` qualifier.
         However, using such functions is highly discouraged as it indicates a class instance is modifiable during member-function execution, which is at odds with Phlex's functional-programming paradigm.

.. only:: html

   .. rubric:: References

.. [CppFunctionDecl] https://en.cppreference.com/w/cpp/language/function
