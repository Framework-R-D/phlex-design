----------------------
Filters and Predicates
----------------------

+----------------------------+-----------------------------------------+------------------------+
| **Filter**                 | Operator (predicate)                    | Output sequence length |
+============================+=========================================+========================+
| :math:`a' = \filter{p}\ a` | :math:`p: A \rightarrow \text{Boolean}` | :math:`|a'| \le |a|`   |
+----------------------------+-----------------------------------------+------------------------+

Any user-defined algorithm or output sink may be configured to operate on data that satisfy a Boolean condition or *predicate*.
The act of restricting the invocation of a function to data that satisfy a predicate is known as *filtering*.
To filter data as presented to a given algorithm, one or more predicates must be specified in a *filter clause*.
Phlex will not schedule a predicate for execution if it is not bound to a filter.

.. todo::

   Define filter clause.
   Many algorithms can specify the same predicate in their filter clauses without executing the predicate multiple times.

Phlex will only schedule a filter for execution if there is at least one non-filter algorithm or output sink downstream of it.
Predicates can be evaluated on (e.g.) run-level data-product sets and applied to algorithms that process data from data-product sets that are subsets of the run (e.g. events).

.. table::
    :widths: 15 85

    +--------------+-----------------------------------------------------------+
    | **Operator** | **Allowed signature**                                     |
    +==============+===========================================================+
    | :math:`p`    | :cpp:`bool function_name(P1, Pn..., Rm...) [qualifiers];` |
    +--------------+-----------------------------------------------------------+


Registration interface
^^^^^^^^^^^^^^^^^^^^^^

.. code:: c++

   class tracks { ... };

   bool select_tracks(Tracks const& ts) { ... }

.. code:: c++

   PHLEX_REGISTER_ALGORITHMS(m, config)
   {
     auto selected_data_scope = config.get<std::string>("data_scope");

     predicate("good_tracks", select_tracks, concurrency::unlimited)
       .sequence( <input_sequence_spec> );
   }
