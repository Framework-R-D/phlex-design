Supported Higher-Order Functions
================================

In Phlex, HOFs transform one family to another:

.. math::
    \ifamily{b}{\text{output}} = \text{HOF}(f_1,\ f_2,\ \dots)\ \ifamily{a}{\text{input}}

where the functions :math:`f_1, f_2, \ \dots` are *operators* required by the HOF.
Note that the output index set :math:`\iset{\text{output}}` is not necessarily the same as the input index set :math:`\iset{\text{input}}`.

In what follows, a family :math:`[a_i]_{i \in \iset{a}}` will often be represented by a single variable :math:`a`.
Whether a variable name (e.g. :math:`a`) represents a family or an operation to a higher-order function will be apparent based on context.

Each HOF below also supports the :math:`\textit{label}:\one \rightarrow L` operator, where the output data layer is user-specifiable.
The :math:`\textit{label}` operator is explicitly mentioned only for those HOFs that require it–i.e. :math:`\text{unfold}` and :math:`\text{window}` [#label]_.

.. toctree::
   :maxdepth: 1

   hofs/transforms
   hofs/observers
   hofs/predicates
   hofs/filters
   hofs/partitioned_folds
   hofs/partitioned_unfolds
   hofs/windows

.. rubric:: Footnotes

.. [#label] The specific rules by which the :math:`\textit{label}` operator can be used are given in :numref:`ch_subsystem_design/data_management:Data Product Management`.
