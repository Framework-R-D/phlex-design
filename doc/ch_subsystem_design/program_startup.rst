Program Startup
===============

The configuration document specifies the modules and sources to be loaded, and provides configuration parameters for each.
The configuration document also specifies which data products are to be written to the output.

Loading each module creates one or more computational nodes.
Loading each source creates one or more provider nodes to be assembled into the data-flow graph.
Once all these nodes have been created, the graph assembly begins.
First, nodes whose output data products satisfy the input requirements of other nodes are connected.

Any input ports that remain unsatisfied after this initial connection step are resolved as follows:

#. If a provider has been configured to deliver the required data product, that provider is connected to the unsatisfied input port.
#. For any input port that remains unconnected, the framework determines whether a provider that reads from I/O—possibly in combination with a data-product translator---can supply the required product.
   If so, the necessary provider and translator nodes are created and added to the graph.
   If no means of satisfying a required input port can be found, program startup fails, as the data-flow graph cannot be made complete.
