-------------------
Partitioned Unfolds
-------------------

+----------------------------------------------------------+----------------------------------------------------+------------------------+
| **Partitioned unfold**                                   | Operators                                          | Output sequence length |
+==========================================================+====================================================+========================+
| :math:`c = \punfold{p}{\textit{gen}}{\textit{label}}\ d` | :math:`p: D \rightarrow \mbox{Boolean}`            | :math:`|c| \ge |d|`    |
|                                                          +----------------------------------------------------+                        |
|                                                          | :math:`\textit{gen}: D \rightarrow D \times C`     |                        |
|                                                          +----------------------------------------------------+                        |
|                                                          | :math:`\textit{label}: \one \rightarrow L`         |                        |
+----------------------------------------------------------+----------------------------------------------------+------------------------+

Unfolds are the opposite of folds, where the output sequence is larger than the input sequence :need:`DUNE 33`.
An unfold can be used for parallelizing the processing of a data product in smaller chunks.

Unfolding predicate
^^^^^^^^^^^^^^^^^^^

Registration interface
^^^^^^^^^^^^^^^^^^^^^^
