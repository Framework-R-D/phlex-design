---
title: Proposed Lifecycle for Requirements
date: 2024-06-20 
---

# Purpose of this document

This document describes the "lifecycle" for requirements for the DUNE framework.
It presents what is in effect a state machine for a requirement, but one that describes only the common paths.
In unusual circumstances, it may be necessary to move a requirement from almost any state to almost any other.
The main limitations on this are the transitions that explicity require DUNE approval.

# The purpose of having these states

We intend to use these states to manage both our (the development team's) process of developing and refining requirements and the process of getting DUNE's feedback and eventual approval of requirements.
Some portions of the state machine describe our internal process, while other portions concentrate on our interations with DUNE.
Note that "the development team" may eventually include groups outside of Fermilab.
Thus part of the purpose is to define a means of communicating with those groups.

# A walk through the lifecycle

A new (candidate) requirement begins in the *draft* state.
This is the state it remains in while a single member, or a few members, of the development team are working on it.
When these authors think the requirement is ready, they move it to the *ready-for-review* state.
An existing requirement may get set back to the *draft* state if it fails an approval review from DUNE.
Under exceptional circumstances, a requirement may be set to the *draft* state from almost any other state.
It is in this state whenever (part of) the development team is working on wording for the requirement.

When the authors of the draft requirement think it is ready, they move it to the *ready-for-review* state.
This indicates that it is ready to be scheduled for review by the development team, and indicates to the project lead that this requirement should be scheduled for review.

When the project lead is ready to conduct a review, he set the requirement to the *in-review* state.
In this state rewording may be done during the review.

In the normal workflow, the result of the review will be putting the (possibly updated) requirement into the *ready-for-approval* state.
This state indicates that the requirement is ready for DUNE's consideration.
In this state the requirement may gain comments from DUNE reviewers, and possibly minor edits.
DUNE may either approve the requirement's wording, thus moving it to the *approved* state, or disapprove it, moving it to the *disapproved* state.

Requirements in the *disapproved* state should generally have one or more comments indicating the problem or problems that need to be fixed.
The requirement would go back to the *draft* state to address the comments.
If the requirement is just totally wrong, it may be moved to the *removed* state.

Requirements in the *approved* state might be then marked as *deferred* to indicate that while the requirement is appropriate, it is of low priority and should not be acted on until some later date.

When a requirement is in the *approved* state, the development team can start working on the tests necessary to determine whether the system meets that requirement or not.
When any such tests are ready for review by DUNE, the requirement should be moved to the *implemented* state.

Requiremens in the *implemented* state are awaiting for DUNE's sign-off that the tests assocaited with the requirement are sufficient to verify the requirement has been met.
When DUNE signs off on the tests, the requirement should be moved to the *verified* state.
If DUNE does not sign off on the tests, comments should be added to the requirement clariying what needs to be fixed -- either in the requirement or in the tess.
The requirement should then be moved to the *rework* state while this fixing is in progress.
When the development team believes the issues raised have been addressed, the requirement should go back to the *implemented* state.

Requirements that get split into multiple requirements should be put into the *removed* state after the new requirements are in the system.
