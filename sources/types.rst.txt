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
