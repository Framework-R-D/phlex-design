---
title: Considerations for registration interface
author:
    - Kyle Knoepfel
date: 2025-03-24
geometry:
    margin=1in
pdf-engine: pdflatex
---

# Introduction

Phlex is adopting a functional programming paradigm that uses higher-order functions (HOFs) whose operators act on sequences of data.
The challenge is to determine how algorithms that serve as operators to HOFs should be registered with Phlex.
The [Meld project](https://github.com/framework-r-d/meld) provides a [fluent interface](https://en.wikipedia.org/wiki/Fluent_interface) for registering algorithms.
It has been observed that the current interface for Meld can lead to confusion, especially for fold operations.
In this document I discuss aspects of how code authors (and job runners) might specify HOFs and the sequences of data on which they operate.

I claim that a fluent interface is desirable and still possible in the context of registering algorithms.
However, I believe that the interface should be adjusted wrt. Meld.

# Using higher-order functions

We focus on three HOFs---the *transform*, the *fold*, and the *unfold*.
Each of these HOFs require one or more *operators* that will be applied to the data presented to the HOF.
In a non-framework context, each HOF might be (a) specified, and (b) **invoked** in C++ as:

``` c++
auto transformed_sequence = transform(tr_op, sequence);
auto folded_result        = fold(f_op, sequence, initializer);
auto unfolded_sequence    = unfold(are_done, uf_op, starting_point);
```

The arguments `tr_op`, `f_op`, `are_done`, and `uf_op` are user-provided operators to the HOFs.
Although it may not seem important to distinguish between the specification and invocation of the HOFs in a non-framework context, such distinction is crucial in a framework context, where the framework is responsible for the invocation of what the code-author/job-runner specifies.

With each HOF above, no explicit iteration loop has been specified---such a loop is internal to the implementation of the HOFs themselves.

## A note about types

Before moving further in our discussion, let's define what we mean by *type*.
A reasonable mathematical description of a type is that it is a mathematical *set* of objects.
For example, the C++ type `int` is an approximation to the mathematical set $\mathbb{Z}$, although there may be technological limitations on what values an object of type `int` can take.[^python]

Suppose, however, that an algorithm $p(n_{POT})$ is configured to operate on an integer $n_{POT}$ that corresponds to the number of protons on target.
In such a case, specifying the function $p$ as $p: \mathbb{Z} \rightarrow R$ (where $R$ is an arbitrary return type) is too permissive.
In a framework context, not all data of type `int` (the equivalent to $\mathbb{Z}$) are suitable for processing by the algorithm $p$.

In this document, we therefore refer to a type as whatever information is necessary to select the correct data objects.
This includes more than just the C++ type `T`; it also includes various labels that identify which kind of `T` is desired.[^restriction]


[^python]: The same discussion applies for Python types.
[^restriction]: In mathematics, imposing further constraints on a function's domain is referred to as [function restriction](https://en.wikipedia.org/wiki/Restriction_(mathematics)).

## Transforms

We begin our discussion with the transform, which takes one user-provided operator $\mathtt{tr\_op}: \mathtt{T} \rightarrow \mathtt{U}$ that is applied to each element of `sequence`.

In a framework context, the user-provided algorithm `tr_op` is the C++ function that is registered.
However, to apply the algorithm to specific labeled data products, the restricted function $f: A \rightarrow B$ is created, where $A = \mathtt{T} \cap L$ and $B = \mathtt{U} \cap Q$.[^types]
Here, $L$ and $Q$ represent the labels of sets that contain the desired data.

As the operator's input argument is of type $A$, the `sequence` presented to the transform is

$$
(a_i | a_i \in A)_{i \in \mathcal{I}} \ .
$$

In words, this is the sequence of all $a$'s subject to the constraint $a \in A$ that can be indexed by the *index set* $\mathcal{I}$.
In a framework context, the index set includes the identifier of each data-product set containing an $a$ object to be processed by the transform.
Specifically, we have:

$$
\text{transform}\left\{f, (a_i | a_i \in A)_{i \in \mathcal{I}} \right\} =
(f(a_i) | a_i \in A)_{i \in \mathcal{I}} =
(b_i | b_i \in B)_{i \in \mathcal{I}} \ .
$$

The transform thus effects the sequence transformation $(a_i) \rightarrow (b_i)$,  where $i \in \mathcal{I}$ for *both* input and output sequences.

To fully specify the transform, we thus need:

1. the algorithm `tr_op` that serves as the transform operator
2. the input sequence element type ($A$, as determined by `tr_op` and restricted by $L$)
3. the output sequence element type ($B$, as determined by `tr_op` and restricted by $Q$)
4. the index set from which to form the input sequence and to which to associate the output sequence.

Examples of index sets could include:

- | `"job"`
  | *the set containing the job ID of the program (resulting in a sequence with only one entry)*
- | `"run/subrun/spill"`
  | *the set of IDs of all spills within the run-subrun-spill hierarchy as seen by the program*
- | `"job/run/*"`
  | *the set of IDs within any run as seen by the program*
- | `"*/spill"`
  | *the set of IDs of all spills seen by the program, regardless of where the spill is in the data category hierarchy*

Although the complete contents of the resulting $(b_i)$ sequence depend on the index set $\mathcal{I}$, an individual element $b_j$ can be obtained using a single-element index set $\left\{j\right\}$ or a much larger index set $\left\{\dots, j-1, j, j+1, \dots\right\}$.
This means that whether the transform is (a) invoked once using a large index set, or (b) invoked many times on smaller index sets that are partitions of the large index set, the result for a given index $j$ will be the same.
In practice, a given transform is likely to be invoked on data that correspond to the entire job and not subsets of the job.

[^types]: The types $A$ and $B$ are each allowed to represent a tuple of multiple values.

## Folds

With a fold, the input `sequence` $(a_i)_{i \in \mathcal{I}}$ is reduced or *folded* into a single result $r_{\mathcal{I}}$, using a binary operator $\mathtt{f\_op}: \mathtt{T} \times \mathtt{C} \rightarrow \mathtt{C}$ with an `initializer` $c_0 \in \mathtt{C}$.

As in the case of the transform, the operator `f_op` must be restricted to the desired label set to select the correct data.
We therefore create a new function $\oplus: A \times \mathtt{C} \rightarrow \mathtt{C}$, where $A = \mathtt{T} \cap L$.
Note that the argument `C` is *not* restricted as it is only the final fold result $r_{\mathcal{I}}$ that receives the label belonging to the label set $Q$.

Mathematically, this looks like[^folding]

$$
r_{\mathcal{I}} = \text{fold}\left\{\oplus, (a_i | a_i \in A)_{i \in \mathcal{I}}, c_0 \right\} =
\bigoplus_{i \in \mathcal{I}}a_i \oplus c_0 =
(a_1 \oplus \cdots (a_{n-1} \oplus (a_n \oplus c_0))) \ .
$$

where $r_{\mathcal{I}} \in R$, with $R = \mathtt{C} \cap Q$, thus restricting the result.

[^folding]: Although a right fold is demonstrated here, the fold ordering does not matter for this discussion.

A fold thus performs a transformation $(a_i)_{i \in \mathcal{I}} \rightarrow r_{\mathcal{I}}$, resulting in a single value corresponding to the index set $\mathcal{I}$.

In contrast to transforms, which generally use an index set corresponding to the entire job,
physicists often keep careful track of fold results at (e.g.) run boundaries.

For example, suppose the index set of a job consisting of two runs can be represented by the union $\mathcal{I}_1 \cup \mathcal{I}_2$.
Assuming $c_0$ is a unit element for the $\oplus$ operator, the separate fold results $r_{\mathcal{I}_1}$ and $r_{\mathcal{I}_2}$ can be combined through the fold $r_{\mathcal{I}_1} \oplus r_{\mathcal{I}_2}$.[^associative]

[^associative]: This also assumes that $\oplus$ is associative (and possibly commutative, depending on the ordering of elements in the index sets).

To fully specify the fold, we thus need:

1. the algorithm `f_op` that serves as the fold operator
2. the input sequence element type ($A$, as determined by `f_op` and restricted by $L$)
3. the output result element type ($R$, as determined by `f_op` and restricted by $Q$)
4. the index set from which to form the input sequence and to which to associate the fold result
5. the initializer $c_0$, or an initializing function $\xi(\text{ID}_{\mathcal{I}})$ that, when invoked with the identifier of the index set $\mathcal{I}$, returns the initializer $c_0$.

## Unfolds

## Takeaways
