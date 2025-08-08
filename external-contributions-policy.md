---
title: Policy Proposal for Contributions With Respect to Platforms and Architectures
updated: 2025-07-30 15:39:52Z
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

## Charge
"Proposed policy for Phlex supporting external contributions for platform _vs_ architecture"—Kyle Knoepfel, Phlex Developers Meeting 2025-07-28

## Definitions
CI/CD
: Continuous Integration/Continuous Development.

External Contribution
: A contribution to Phlex code from someone other than a core Phlex/FORM developer.

Official Support
: A commitment from the core Phlex developers to support the feature going forward.

PR
: Pull Request: [GitHub](https://github.com)'s term of art for a packaged code contribution to a specific project.

Stakeholder Requirement
: A stakeholder-approved requirement on Phlex duly entered into and managed within the Jama system.

System
: A specific platform/architecture combination.

## Rationale
At the current time, the following systems have official support:
* AlmaLinux 9 on `x86_64_v3` as defined by the Python [Archspec](https://archspec.readthedocs.io/en/latest/) package. 

It is envisaged that this list will expand as new stakeholder requirements are added. The Phlex core development team commits to maintaining the Phlex software and ensuring ongoing satisfaction of all stakeholder requirements, including required systems.

It is additionally envisaged that the Phlex project will receive PRs representing external contributions. Accepting these contributions can risk increasing the support burden on the core developers beyond sustainable levels, especially during the design and implementation phases of the project. This risk is especially great with respect to contributions that introduce unsupported systems.

It is therefore necessary to constrain the parameters of such contributions with a policy such as the one proposed herein.

## Policy
### External Contributions Introducing Unsupported Platform/Architecture Combinations
1. Shall not inhibit Phlex's ability to satisfy requirements for officially supported systems.
2. Shall conform to all other Phlex policies for PRs, including (but not limited to) those concerning:
	1. CI/CD tests
	2. Test coverage
	3. Supported compilers
	4. Coding style
	5. Language standard
	6. Documentation

	where applicable.
3. Shall include necssary additions to CI/CD configuration to support ongoing automatic verification of _all_ Phlex code on the new system.
4. Shall include provision (where not already present) of the necessary hardware for ongoing CI/CD testing of _all_ Phlex code on the new system.

### Exception Conditions
1. In the event that CI/CD test failures or other anomalous behavior can be traced to an external contribution after it has been accepted:
	1. The contributor(s) shall be notified of such issues and asked to resolve them within a reasonable time period reflecting the scope of the failure (e.g. failures affecting officially supported systems shall be considered urgent).
	2. If that failure is not remediated timely, the minimum action shall be taken by the core developers to mitigate the effect of the failure on Phlex's ongoing ability to satisfy stakeholder requirements. 
	3. Such minimal action may include disablement or removal of the external contribution.
	4. The external contribution may be restored via a subsequent PR conforming with this policy.
2. In the event of the unavailability of necessary hardware for CI/CD on the introduced system:
	1. Affected contributors shall be notified and asked to resolve the situation.
	2. CI/CD shall be reconfigured to avoid operations guaranteed to fail due to the unavailability.
	3. If such failure is deemed by the core developers to be unlikely to be resolved, affected code shall be removed from the repository.