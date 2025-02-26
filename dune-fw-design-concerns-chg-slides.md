---
title: "Framework Design, You Are Doin' Me A Concerns"
author: "Chris Green, FNAL"
date: "2024-08-08"
theme: MFP
aspectratio: 1610
colorlinks: true
---

## General ##

* We can design a framework to meet DUNE's needs, or we can design a
  framework to conform to time, budget, and effort/expertise constraints
  as currently understood; I don't believe that we can do both.

* We are necessarily starting a design while dealing with an information
  and/or collaboration deficit with respect to interactions with DUNE,
  especially:
  * use cases and/or user stories,
  * I/O backend requirements, and
  * whole categories of requirements (e.g. configuration language).

  Having to rework significant parts of the design after producing
  it---or even during implementation---against this backdrop is a
  practical certainty.

## General ##

* In order to justify the design, we may need to produce significant
  demonstrations to introduce and/or illustrate new ideas, and we will
  have to tie each aspect of the design back to requirements, use cases,
  and/or user stories.

* Along with the design, we need to have a comprehensive and specific
  plan of software engineering requirements---including (but not limited
  to) language, standard and external library use guidelines, testing,
  verification, validation, requirement tracing and coding style---both
  for core framework, the I/O API, and user plugin and algorithm
  code. This plan must be implementable, sustainable, and enforceable.

## Algorithms ##

* I believe we need a solid, justifiable separation of concerns between
  "plugin definition" code and the algorithm proper.

* We need to define algorithms as being as close to functional as
  possible. The further we fall short of this, the more of a problem we
  are likely to face with respect to reproducibility.

* We will need to consider very carefully how to provide straightforward
  ways to implement in algorithms functionality previously provided by
  services.

* We must find a workable way to record and perserve design
  discussions---and the intermediate and final results thereof---in
  order to ensure we continue in the right direction from both the
  developers' and experiment's perspectives.

## Algorithms ##

* The framework will need to obtain certain information from an
  algorithm before it can provide the data it needs. In most cases this
  will involve calculations on some data before knowing what other data
  are needed, in which case the algorithm can/should be
  factorized.
  
  However, in the case where the algorithm's data requirements must be
  determined using configuration, the need for factorization could be
  seen as overly burdensome. The solutions for this would seem to be:
  * allowing multiple entry points for an algorithm (see previous point
    re functional character of algorithms), or
  * allowing a preliminary "algorithm" receiving the same configuration
    as the main---but no data products---and returning only data
    _specifications_, which can be invoked during the plugin steering
    step defined by the registration code.

## Algorithms ##

* I believe we need to allow the same algorithm to be used by multiple
  plugins for maximum reuse capability in the absence of stateful
  algorithms, because (e.g.):
  * the results need to be composed differently, or
  * configuration must be used in a different way to determine the
    algorithm input specifications.

* We must have a coherent plugin/algorithm specification that is capable
  of handling:
  * physics algorithms,
  * defintion of data sets, data families, and adjacency,
  * definition, receipt, and production of data product and other
    metadata.

## I/O ##

* I believe we will need a data description language---and possibly
  additional functionality besides---in order to specify the in-memory
  structure and gross persistence characteristics of data products. This
  language will need to be able to handle:
  * Data products intended for diagnostic purposes only (never written).
  * Intermediate data products (not written by default).
  * Precious data products (written by default).
  * Inter/intra-product references to data products, their members, and
    elements and slices of collections (both data products and members).

* We must be able to use the information specified above to generate
  in-memory representations in each supported algorithm implementation
  language.

* We must enable the specification of algorithms' data product, and
  metadata input requirements (_cf_ `art::InputTag`) such that they may
  be found efficiently either from input sources, or by invoking the
  appropriate algorithm.
