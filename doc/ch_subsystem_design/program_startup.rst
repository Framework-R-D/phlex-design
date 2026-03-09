Program Startup
===============

At program startup, the application reads a configuration document that specifies the workflow to be executed.
The configuration document specifies the modules and sources to be loaded, and provides configuration parameters for each.
Optionally, the configuration document also specifies which data products are to be written to the output.
Optionally, the configuration document also specifies what input file or files are to be read, including both physics data files (e.g. ROOT or HDF5 files in the framework's format) and ancillary data files (e.g GDML files specifying a geometry).
N.B.: this is an incomplete list of the aspects of the program that are specified by the configuration document.

N.B.: This has not description, yet, of where the *driver* configuration comes in.


Loading each module creates one or more *computational* nodes.
Computational nodes are the nodes that do physics reconstruction or simulation tasks, as opposed to the reading and writing of data.
Loading each source creates one or more *provider* nodes to be assembled into the data-flow graph.
Provider nodes are the nodes that either read data products or create data products from an input that is only a data cell index.
Once the computational and provider nodes have been created, the graph assembly begins.

First, computational nodes whose output data products satisfy the input requirements of other computational nodes are connected forming a graph.
N.B.: we need to determine whether translator nodes can be introduced at this step.

Next, any input ports that remain unsatisfied after this initial connection step are resolved by connecting them to provider nodes.
If a provider has been configured to deliver the required data product, that provider is connected to the unsatisfied input port.
N.B.: we need to determine whether translator nodes can be introduced at this step.


For any input port that still remains unconnected, the framework determines whether a provider that reads from I/O (possibly in combination with a data-product translator) can supply the required product.
If so, the necessary provider and translator nodes are created and added to the graph.
If no means of satisfying a required input port can be found, program startup fails, as the data-flow graph cannot be made complete.

Finally, for any created data products that the configuration specifies should be written to the output, a preserver node is created.
The preserver node is connected to the computational node that produces the data product, and is configured to write that data product to the output.

