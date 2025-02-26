---
title: Chunkable data products and the algorithms that love them
author:
    - Marc Paterno
    - Saba Sehrish
date: 2023-10-05
geometry:
    margin=1in
output:
    pdf_document:
        latex_engine: xelatex
classoption:
    - letterpaper
    - 9pt
numbersections: true
header-includes:
    - \usepackage{graphicx}
    - \usepackage{microtype}
---

# Introduction

This document discusses some design ideas for the data product model related to the support of chunkable data products.
Because chunkable data products are a solution to a the processing of data products that can be too large to fit into memory, we also describe some of the features of processing such data products.
We also discuss some ideas regarding the writing and reading of such data products.


# What is the nature of a chunkable data product?

A chunkable data product is any data product that can be split into chunks with the intent of being able to process the chunks separately.
The canonical example in C++ for such a data product is a `std::vector<X>` where `X` is some defined type.
This is not the only example.
Another could be a struct X that contains several parallel vectors, each of which is of the same length.
The Python equivalents of these could be created with `numpy` `ndarray`s rather than with `std::vector`.

Note that not all data products that can be chunked *must* be chunked.
The option to chunk the data is available if the data product is chunkable, but that option might not be used.

We will call the separate pieces of a chunkable data product that has been chunked *chunks*.

The framework must be able to identify which types can be chunked.
If we use our own DDL this can be done directly through the DDL.
If we use C++ and Python as our DDLs then the framework needs some mechanism by which we identify chunkable types.

We do not call every non-primitive data type chunkable.
For example, the C++ struct defined by:


```
struct X {
    int a;
    double b;
}
```

is not chunkable.
While it may be possible, and even useful, to write files in a format that allows the reading of only `a` or `b`, this is not what we mean by *chunking* in the context of a data product.
The driving use case (the processing of data products too large to hold in memory) is not based on have a struct with a huge number of fields which in aggregate are too large to hold in memory.
The real use cases involve data structures that a sequence of some element type, with the sequence length being large.
The element type can be primitive or non-primitive.
The sequence may be stored in array-of-struct or struct-of-array form.


# How do we write a chunkable data product?

* How do we tell the IO system that we need to chunk the data product that we want to write?

# How do we read a chunkable data product?

* How do we tell the IO system that we need to chunk the data product that we want to read?


# What higher order functions that framework will provide that can take advantage of the chunkable data product as input?

* this may include what is expected from user defined algorithms

* requirements may vary with the kind of higher order function, e.g. for transformation

# What higher order functions that framework will provide that can take advantage of the chunkable data product as output?


# What higher order functions that framework will provide that can take advantage of the chunkable data product both as input and output?
