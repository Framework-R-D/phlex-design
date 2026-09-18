---
---
Given the report at ../phlex-design/phlex-art-comparison-report.md, please consider how to proceed with option C of the candidate C++ shapes ("Opaque token resolved by the framework before algorithm dispatch").

1. Use project memories and semantic_search as necessary to obtain context from the current project, ../phlex-design/, and context from the Art suite: ../../art-devel/{art-root-io,gallery,canvas_root-io,art,canvas,messagefacility,fhicl-cpp,cetlib,hep-concurrency}.
2. Focus on the POV of the algorithm author.
3. Do _not_ read any other documents from docs/dev/persistent-references: treat this as a _de novo_ analysis independent of prior attempts.

4. Ask further questions as necessary for clarification or at decision points

Also, please consider at what point in the process we would need to make a choice of DSL and/or have a custom DSL designed/implemented.

Note: if delegating to e.g. explore, explore-high, be sure to specify model and effort as–while Ask Sage-provided models are available, tokens are expensive and the quota is extremely limited, so the vast majority of work should be undertaken by models provided by fnal-ow (e.g. gpt-oss-120b and qwen/qwen3-coder-next—the latter having caveats) and use of paid models should be sparing, judicious, and with focussed context. Use the model-guru skill to aid delegation and model/effort choice.

Other details that might clarify/anticipate/preempt decision points:

1. Q: Do any target algorithms need to output a persistent reference or association?

   A: yes.

2. Q: Can all required references be satisfied within the same data cell (no cross‑cell look‑ups)?

   A: We will likely eventually need cross-cell lookup: please consider this as a staged objective.
   
3. Q: Is the association topology (e.g., one‑to‑many, many‑to‑many) already describable in the current jsonnet/FHiCL workflow configuration?

   A: That would seem to be an attribute of the referenced data and/or algorithm - I'm not sure that would be suitable as runtime configuration.
   
4. Q: Do you have an existing IDL or code‑generator pipeline for data‑product schemas, or would you need to introduce a Phlex‑native schema DSL?

   A: We do not. A previous analysis indicated a Phlex-native DSL would be necessary, but we have not recently refreshed the survey of available off-the-shelf/customizable options.
   
5. Q: What is the exact cardinality of the reference you need to output? (e.g. one‑to‑many Wire → Hit, many‑to‑many Hit ↔ TruthParticle, or a simple element reference.)

   A: Depending on the client algorithm, we will need to support references to and associations between whole products, to elements within collection products, and from element to element, with or without supplemental data in the association case.
   
6. Q: Will any future algorithm need to forward an incoming reference unchanged to its output (i.e., ref_in → ref_out)?

   A: Yes.

7. Q: Do you prefer the schema DSL to be YAML (human‑readable) or JSON (easier to validate with a schema validator)?

   A: We have a slight pereference for YAML if that doesn't cause practical issues (jsonnet is also an option). One random nugget: the ability to evolve the description of a specific product, with the ability to describe how to handle input data containing an earlier version of the product.

8. Q: For cross‑cell references, how will cells be identified at runtime? (Static cell‑names, numeric IDs, hierarchical paths?)

   A: Layer name (e.g. spill) and cell identifier (e.g. spill number) within  that layer.
   
9. Q: Are there any performance constraints (e.g., maximum latency for reference resolution) that would push us to batch‑resolve all associations in a single pass versus on‑demand per‑algorithm?

   A: The algorithm should be able to declare at registration time which references to or within a product will be required (or not), but resolution of required references should be complete before the algorithm is executed by the scheduler.

Further details guiding the work product:

1. I will need a project-architecture -> developer-level design document as a Markdown file representing the results of this analysis, including justifications for specific decisions taken. Where appropriate, note potential but-not-currently-envisaged use cases that might be made difficult or impractical  by a given implementation choice.
2. If possible, produce UML design diagrams at the level of detail necessary to enhance understanding at various points. Use plantuml (`/opt/homebrew/bin/plantuml`); additional plugins/tooling can be installed on request. All code blocks in the document should be fenced, although short inline snippets are acceptable.
3. Test implementations to the extent possible (PR threshold is 80%), using mocking sparingly but as necessary (mimic-cpp is available as a build-time dependency through `FetchContent()`).
4. Be very sure to limit the context and scope of tasks given to qwen-coder-next (use the coder-qwen subagent) given its propsensity for "doing its own thing," and always vet its work product thoroughly. Ask Sage Luna is available and should be used if qwen-coder-next goes off the rails more than once or twice on a given task.
