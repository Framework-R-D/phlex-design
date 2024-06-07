# Implications of DUNE v0 rqeuirement 009 #

## Requirement text

Framework output files MUST be readable outside the framework with minimal overhead (i.e. bare ROOT).

## Subsequent correspondence

<dl>
<dt>`[Devs]`</dt>
<dd>By framework output file, do you mean the default framework-produced ROOT file that contains all physics data + framework metadata?  Does bare ROOT mean using the root command line without any extra dictionaries than what are already provided by ROOT?  Or does it mean the root command line with extra dictionaries?  Or something else?</dd>

<dt>`[DUNE]`</dt>
<dd>Yes. The preferred behavior is that framework data files (ARTROOT like files) are easily interrogated by users using ROOT without external dependencies or extra dictionaries.</dd>

<dt>`[Devs]`</dt>
<dd>For what purposes would you like to read framework-produced data outside of the framework? The extent to which a framework output file can be read outside of a framework depends heavily on the kinds of data to be read and the user-provided layouts of those data. We imagine there are different kinds of data from an output file one may want to read (e.g.):

    * Detector raw data
    * Derived data products
    * Simulation results
    * Data provenance
    * Configuration data
    * User-provided ancillary data
    * Persisted (uni- and bi-)directional associations between data products (e.g. the kind of information stored in art::Ptr<T>s and art::Assns<A, B, D>s)

    If you can provide specific use cases, we can evaluate the feasibility of this request.</dd>

<dt>`[DUNE]`</dt>
<dd>_\[no further response to date\]_</dd>
</dl>

## Discussion

Requiring that, "framework data files (ARTROOT like files) are easily interrogated by users using ROOT without external dependencies or extra dictionaries" places severe restrictions on the format and content of those files. If we consider specifically the example of ROOT, this means that:

1. While the raw data from the file and hierarchical relationships (e.g. class data members) remain accessible, interface allowing the manipulation of those data (e.g. polar _vs_ cartesian coordinates, or pseudorapidity) are not. Users of the Art framework have always been _encouraged_ to define data products without such interface, but req. 009 would imply that when reading the data with bare ROOT, any such interface would not be available.

1. Art-style I/O-format-agnostic references and associations (e.g. `Ptr`, `Assns<A, B, D>`) are not navigable with bare ROOT.

   In the old `TTree`-based format used by Art, ROOT supports references via bare pointers. However, this has implications for the framework's in-memory representations and would make satisfying other requirements significantly harder (e.g. req. 002, "The framework MUST separate the persistent data representation from the in-memory representation seen by algorithms," and, req. 051, "The framework, through the I/O modules/plugins, MUST provide for the capability of reading from and writing to various types (e.g. ROOT, HDF5, 'object stores')"). Historically frameworks have often forbidden (Art) or severely restricted (CMSSW) the use of C++-style pointers and references in persistent data. This would also appear to conflict with req. 007, "Modules within the framework MUST be allowed to contain code from a variety of languages."
   
   In the newer ROOT `RNtuple` format, storage only of basic types is expected. Consequently, C++-style pointers and references are explicitly disallowed, and while it would be possible to _store_ IO-format-agnostic references _a la_ `art::Ptr`, actually using them to access the referenced data would require external code. ROOT-native references _a.k.a._ links are evisaged (eventually) for RNTuple, but neither the details of that feature nor its timescale for implementation are currently defined.

1. In all likelihood, whether _any given_ framework-produced data file satisfies req. 009 will depend on the user-specified structure of the data, and the exact meaning of the phrase 'easily interrogated'—how, and to what end? In any event is unlikely that any useful manifestation of framework-originated metadata (e.g. provenance tracking) would satisfy req. 009 while remaining useful for the purpose for which it was created.
