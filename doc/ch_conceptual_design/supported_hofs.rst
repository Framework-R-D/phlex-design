Supported Higher-Order Functions
================================

In Phlex, HOFs transform one sequence to another:

.. math::
    \isequence{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \isequence{a}{\text{input}}

where the functions :math:`f_1, f_2, \ \dots` are *operators* required by the HOF.
Note that the output index set :math:`\iset{\text{output}}` is not necessarily the same as the input index set :math:`\iset{\text{input}}`.

In what follows, a sequence :math:`[a_i]_{i \in \iset{a}}` will often be represented by a single variable :math:`a`.
Whether a variable name (e.g. :math:`a`) represents a sequence or an operation to a higher-order function will be apparent based on context.

.. toctree::
   :maxdepth: 1

   hofs/transforms
   hofs/filters
   hofs/observers
   hofs/partitioned_folds
   hofs/partitioned_unfolds
   hofs/windows
