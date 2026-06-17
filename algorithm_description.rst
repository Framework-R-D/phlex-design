Algorithm Description
=====================

The Algorithm Description subsystem collects and provides information about all user-registered code (computational algorithms, providers, preservers, and translators) that can appear in the workflow graph.
This information is available both as structured data (for tools and the framework) and as human-readable text (for users).

Responsibilities
----------------

The subsystem is responsible for:

- **Providing information to the user as to what an algorithm can do** — human-readable descriptions of algorithm capabilities, behavior, and use cases.
- **Providing information about configuration parameters** — schema, defaults, validation rules, and documentation for each configurable parameter.
- **Supporting descriptions of all user-registered code** — extensible to any plugin type (computational, provider, preserver, translator) and language (C++, Python).

The subsystem answers questions such as:

- "What does this algorithm/node do?"
- "What are its inputs and outputs?"
- "What resources does it require?"
- "What configuration parameters does it accept, and what are their constraints?"
- "How do I configure this algorithm?"

Descriptor Data Model
---------------------

The subsystem defines a canonical descriptor object for each registered algorithm, containing:

**Identity**
  - Stable algorithm identifier (module + symbol + version)
  - Display name
  - Category (computational/provider/preserver/translator)
  - Language (C++/Python)

**Human-readable description**
  - Short summary (one-liner)
  - Detailed description (what it does, capabilities, use cases)
  - Author/maintainer information (optional)

**Signature** (derived from :numref:`ch_conceptual_design/algorithms:Algorithms`)
  - Ordered list of data-product parameters, each with:
    - Parameter kind (``P const&``, ``P const*``, ``P``, ``phlex::handle<P>``)
    - Concrete type name (fully qualified)
    - Associated data-product concept identifier (if available)
    - Parameter documentation
  - Ordered list of resource parameters, each with:
    - Type name
    - Access mode (read-only or read-write)
    - Parameter documentation
  - Return type:
    - Created data-product type (``T`` or ``std::unique_ptr<T>``)
    - Return documentation

**Ownership and concurrency contracts**
  - Ownership model (framework-owned; algorithm must not retain hard references)
  - Concurrency safety (whether safe under configured concurrency)
  - Member function const-ness (``const`` recommended; non-``const`` discouraged)

**Configuration schema**
  - Parameter list with types, defaults, and required/optional status
  - Validation rules (ranges, enums, structural constraints)
  - Documentation string for each parameter
  - Examples of valid configurations

**Provenance**
  - Metadata to be attached to produced data products (creator, suffix, etc.)

Descriptor Production
---------------------

Descriptors are produced at registration time and capture both structural information (signature, types) and human-readable metadata.

**C++**

- Signature information is extracted at registration time; the registration API templates already deduce types.
- Stored information includes:
  - Demangled type names
  - Parameter kinds (reference, pointer, value, handle)
  - Return ownership model
  - Resource access modes
- Human-readable metadata is captured from:
  - Optional registration-time annotations (e.g., ``description(...)``, ``param_doc(...)`` helpers)
  - Source code comments (if available via reflection)

**Python**

- Signature information is extracted via reflection and type annotations.
- Fallback mechanisms include:
  - Explicit annotation objects in registration
  - JIT wrappers (e.g., Numba) that expose signatures
  - ``ctypes`` bindings with explicit signature metadata
- Human-readable metadata is captured from:
  - Docstrings (PEP 257)
  - Type hints
  - Optional registration-time annotations

Interfaces
----------

**Registration-time API**

The subsystem provides builders and helpers to make it easy to attach descriptors to registered nodes, including:

- Descriptor construction from signature information
- Helpers to add human-readable descriptions, parameter documentation, and configuration examples

*Selector API**

The subsystem provides queries to retrieve descriptors by:

- Algorithm identifier or name
- Input/output type
- Data-product concept
- Configuration parameter
- Capability or keyword

**User-facing reporting**

The subsystem provides tools for users to discover and understand registered algorithms:

- CLI command ``phlex describe <node|algorithm>`` — human-readable output of algorithm capabilities, signature, and configuration
- CLI command ``phlex list-algorithms [--filter ...]`` — list all registered algorithms with brief descriptions
- JSON/YAML export — for tooling and programmatic access
- Documentation generation — Sphinx integration to generate reference pages
- Configuration validation — with helpful error messages explaining constraints and valid values

**Validation hooks**

The subsystem validates that registered algorithms satisfy conceptual constraints:

- At least one data-product parameter is required
- Return type must model a created data-product type (``T`` or ``std::unique_ptr<T>``)
- Ambiguous ownership returns (bare pointers, references) are forbidden
- Non-``const`` member functions trigger warnings
- Runtime configuration is validated against the schema with clear error messages

Integration with other subsystems
----------------------------------

- **Plugin Management**: Descriptors are discovered and loaded alongside modules.
- **Configuration**: Configuration schema is consumed to validate runtime configuration; the subsystem provides user-friendly validation messages.
- **Task Management**: Signature and resource information is used to schedule algorithms safely (e.g., enforcing resource write locks).
- **Data Product Management / Metadata Management**: Descriptor information is used to label produced data products and support data-product lookup.
- **Data Modeling**: Concept identifiers and translation availability information (from descriptors) inform translator insertion decisions.

Open issues
-----------

- **Python signature reliability**: Ensuring that Python type annotations and docstrings are reliably available and correctly interpreted.
- **Concept identifier availability**: Mapping concrete types to data-product concepts, especially for legacy code.
- **Versioning and backward compatibility**: How descriptors evolve as algorithms are updated.
- **Configuration validation error quality**: Providing clear, actionable error messages when configuration is invalid.
