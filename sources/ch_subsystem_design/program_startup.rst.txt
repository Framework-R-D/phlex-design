Program Startup
===============

At program startup, the application reads a configuration document that specifies the workflow to be executed.
The configuration document specifies the modules and sources to be loaded, and provides configuration parameters for each.
Optionally, the configuration document also specifies which data products are to be written to the output.
Optionally, the configuration document also specifies what input file or files are to be read, including both physics data files (e.g. ROOT or HDF5 files in the framework's format) and ancillary data files (e.g GDML files specifying a geometry).
The configuration document also specifies the configuration of a *driver*, which determines which data cells will be processed by the program.
[#incomplete-config-list]_

Loading each module creates one or more *computational* nodes.
Computational nodes are the nodes that do physics reconstruction or simulation tasks, as opposed to the reading and writing of data.
Loading each source creates one or more *provider* nodes to be assembled into the data-flow graph.
Provider nodes are the nodes that either read data products or create data products from an input that is only a data cell index.
Once the computational and provider nodes have been created, the graph assembly begins.

First, computational nodes whose output data products satisfy the input requirements of other computational nodes are connected forming a graph.
For each input port on each computational node, a search for a matching output port on another computational node is performed.
Each input port has associated a creator name, a product suffix, a data layer, a concrete data-product type and a data-product concept.
For a successful match, there must be a match for the creator name, product suffix, data layer and data-product concept.
If the concrete data-product type of the output matches the input, then the ports are connected.
If the concrete types do not match, then a translator node can be inserted to convert the output concrete type to the required input concrete type.

Next, any input ports that remain unsatisfied after this initial connection step are resolved by connecting them to provider nodes.
A similar matching process is performed to connect the remaining input ports to the output port of each configured provider node.

For any input port that still remains unconnected, the framework determines whether a provider that reads from IO (possibly in combination with a data-product translator) can supply the required product.
If so, the necessary provider and translator nodes are created and added to the graph.
If no means of satisfying a required input port can be found, program startup fails, as the data-flow graph cannot be made complete.

Finally, for any created data products that the configuration specifies should be written to the output, a preserver node is created.
The preserver node is connected to the computational node that produces the data product, and is configured to write that data product to the output.

.. rubric:: Footnotes

.. [#incomplete-config-list] This list does not describe all the configuration options.
