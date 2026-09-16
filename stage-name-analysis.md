# Phlex stage-name terminology analysis

## Recommendation

Retain **stage name**, while defining it more precisely as a **product-provenance namespace assigned to one Phlex processing stage**.

A concise documentation definition is:

> A stage name identifies the Phlex processing stage that produced a product. It is part of a product's identity, allowing products with the same creator, type, suffix, and layer from different Phlex executions to be distinguished and selected.

This is better than defining it merely as the "name of an invocation." An invocation can suggest one job attempt, while the value should normally be stable across equivalent reruns of the same processing stage. For example, `simulation`, `reconstruction`, `calibration`, and `selection` are meaningful stage names. A timestamp, host name, or batch-job ID generally is not.

## Assessment of candidates

| Term | Assessment |
| --- | --- |
| **stage name** | Best of the presented options. It naturally suggests a recognizable portion of a larger processing pipeline and accommodates multiple algorithms executing internally as a graph. It is familiar in scientific data processing and ETL. Its implied ordering should be documented as conventional rather than enforced. |
| **step name** | Less suitable. "Step" usually means one atomic operation, task, or algorithm invocation. In Phlex, one framework execution contains many graph nodes and algorithms, so users may reasonably expect a step to be smaller than the entity being named. |
| **phase name** | Less suitable. "Phase" commonly denotes temporal framework lifecycle states, such as initialization, configuration, event processing, and finalization. This risks conflict with Phlex's layered and hierarchical execution concepts. |
| **workflow name** | Avoid. A workflow is normally the whole declarative composition of work, and may include multiple Phlex invocations. Nextflow, Snakemake, CWL, and Airflow all give "workflow" an orchestration-level meaning. |
| **graph name** | Avoid for user-facing vocabulary. It describes the implementation or execution model rather than the scientific or processing meaning of the production. It would also be ambiguous if a configuration contains subgraphs or dynamically constructed graphs. |
| **process name** | Semantically familiar to art and CMSSW users, but correctly rejected because "process" is overloaded: OS process, C++ process, dataflow process, and possibly a Phlex algorithm or process node. |

## Alternative terminology

No alternative is clearly better than **stage name**, but two are useful distinctions:

| Alternative | Assessment |
| --- | --- |
| **processing-stage name** | Best long-form documentation term. It makes the intended meaning explicit while retaining the concise configuration and API term `stage`. For example: "Each product is associated with its creator, layer, and processing stage." |
| **provenance label** | Technically precise: the field identifies a product's producing context. However, it is abstract and may imply a complete provenance record rather than one provenance coordinate. Best used to explain the purpose of a stage name, not as the primary term. |
| **run name / execution name** | Accurate only if it identifies an individual runtime attempt. That would be a different concept from a stable production identity. Reserve it for job or run metadata if Phlex later records unique executions. |
| **pass name** | Familiar in HEP, especially for reconstruction or calibration passes. However, it strongly implies repeated or sequential data passes and is too domain-specific for general Phlex use. |
| **production name** | Appropriate for a higher-level data-production campaign or dataset release, but likely too coarse. One production can contain multiple stages. |

## Semantic boundary

Establish three distinct terms:

| Concept | Suggested term | Example |
| --- | --- | --- |
| A multi-execution scientific pipeline | **workflow** | simulation -> reconstruction -> analysis |
| One configured Phlex framework execution and its product namespace | **processing stage** / **stage** | `reconstruction` |
| One particular launched attempt | **run** or **execution** | batch job `48291`, retry `2` |

This vocabulary avoids the main ambiguities:

```text
The workflow runs the reconstruction stage twice.

Both runs belong to the same stage and therefore produce products
whose provenance identifies them as reconstruction products.

The two runs can have distinct run IDs, logs, timestamps, and configuration
digests without changing the products' stage name.
```

If Phlex permits a stage to consume products from an earlier stage, selectors can read naturally:

```text
Select tracks produced by creator "track_finder"
in the "reconstruction" stage.
```

That is clearer than "select tracks from graph `reconstruction`" or "from execution `reconstruction`."

## Comparable systems

- **art** uses a **process name** as a product-provenance identity. Phlex's stage name fills essentially the same semantic role while avoiding the overloaded word "process."
- **CMSSW** constructs a named `cms.Process("RECO")`; names such as `HLT` and `RECO` identify meaningful processing contexts. This is very close conceptually, although CMSSW retains the overloaded term "process." The CMSSW configuration lesson describes names such as `HLT` and `RECO` as meaningful process names.
- **Nextflow** distinguishes **workflows** from **processes**, and its fully qualified process names distinguish calls such as `flow1:tick` and `flow2:tick`. This reinforces that "workflow" is composition-level terminology, not the name of one execution's provenance namespace.
- **Snakemake** uses **workflow**, **rule**, and **job**. A rule is a reusable transformation definition; a job is one concrete instantiation. Neither is a good direct replacement for Phlex's stage concept.
- **Common Workflow Language (CWL)** similarly uses **workflow** and **step**. A CWL step is a component within a workflow, supporting the conclusion that "step name" would be too fine-grained for a complete Phlex execution.
- **Apache Airflow** uses **DAG**, **task**, and **DAG run**. "Run" denotes a particular execution instance, which is useful evidence against using "run name" for a stable product identity.
- Production systems such as Pegasus generally distinguish workflows, transformations, jobs, and workflow runs; they likewise separate orchestration structure from a concrete run instance.

## Suggested naming policy

Use these conventions in user-facing configuration, diagnostics, and documentation:

- Configuration and API field: `stage`
- Formal term: **stage name**
- First-use expansion: **processing-stage name**
- Purpose statement: "The stage name is a product-provenance namespace."
- Avoid calling it a workflow, graph, job, run, or execution name.
- Recommend semantic, stable values such as `simulation`, `reconstruction`, `calibration`, `analysis`, or `validation`.
- Do not imply that stages must be linearly ordered; a stage name labels provenance and is not itself a scheduling constraint.

This preserves the existing Phlex terminology, gives users an intuitive mental model, and leaves room for a future explicit run or execution identifier without overloading the stage field.

## Sources

- [art framework repository](https://github.com/art-framework-suite/art)
- [Phlex design repository](https://github.com/Framework-R-D/phlex-design)
- [CMSSW configuration lesson](https://cms-opendata-workshop.github.io/workshop2021-lesson-cmssw/05_configuration/index.html)
- [Nextflow workflows documentation](https://nextflow.io/docs/latest/workflow.html)
- [Snakemake rules documentation](https://snakemake.readthedocs.io/en/stable/snakefiles/rules.html)
