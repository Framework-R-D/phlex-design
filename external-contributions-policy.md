---
title: Policy Proposal for External Contributions
updated: 2025-08-08 19:30:00Z
created: 2025-07-30 13:51:58Z
latitude: 41.85002840
longitude: -88.31257380
altitude: 0.0000
tags:
  - architectures
  - ci
  - contributions
  - phlex
  - platforms
  - policy-proposal
---

## Definitions

CI/CD
: Continuous Integration/Continuous Development.

Community Support
: CI/CD configuration and appropriate hardware are provided to monitor the ongoing functionality of Phlex on systems without official support. Issues with CI/CD for such a system shall have support at an institutional (rather than individual) level.

External Contribution
: A contribution to Phlex code from someone other than a core Phlex/FORM developer.

Official Support
: A commitment from the core Phlex developers to support the feature going forward.

PR
: Pull Request—[GitHub](https://github.com)’s term of art for a packaged code contribution to a specific project.

Stakeholder Requirement
: A stakeholder-approved requirement on Phlex.

System
: A specific platform/architecture combination.

Unsupported System
: A system without either official or community support

## Rationale

At the current time, the following systems have official support:

- AlmaLinux 9 on `x86_64_v3` (or newer X86-64 architectures) as defined by the Python [Archspec](https://archspec.readthedocs.io/en/latest/) package.

It is envisaged that this list will expand as new stakeholder requirements are added. The Phlex core development team commits to maintaining the Phlex software and ensuring ongoing satisfaction of all stakeholder requirements, including required systems.

N.B. `x86_64_v2` is explicitly *not* supported due to requirements of some third party dependencies.

It is additionally envisaged that the Phlex project will receive PRs representing external contributions. Some of these PRs may provide or affect Phlex behavior on officially or community supported systems, or on unsupported systems.

Accepting external contributions may risk increasing the support burden on the core developers beyond sustainable levels, especially during the design and implementation phases of the project. This risk is especially great with respect to contributions that introduce or affect unsupported systems.

It is therefore necessary to constrain the parameters of external contributions with a policy such as the one proposed herein.

## Policy

### Acceptance of External Contributions

External contributions to Phlex are encouraged so long as:

  1. They do not inhibit Phlex’s ability to satisfy requirements for officially supported systems.

  2. They conform to all other policies for the appropriate Phlex repository, such as those concerning:

     1. CI/CD tests on officially and community supported systems
     2. Test coverage
     3. Supported compilers
     4. Coding style
     5. Language standard
     6. Documentation

     where applicable.

### Post-Acceptance Responsibilities

In the event that CI/CD test failures for any Phlex repository (or other reported anomalous behavior) can be traced to an external contribution *after it has been accepted*:

1. The contributor(s) will be notified of such issues and asked to resolve them within a reasonable time period reflecting the scope of the failure (e.g. failures affecting officially supported systems shall be considered urgent).
2. If any affected system is community-supported, the responsible party for that system's support will also be contacted.
3. If that failure is not addressed in a timely manner, the core developers will take the minimum action required to continue development that satisfies Phlex’s stakeholder requirements.
4. Such minimal action may include disabling or removing the external contribution, or disabling the CI/CD for community-supported systems.
5. The external contribution may be restored via a subsequent PR conforming with this policy.
