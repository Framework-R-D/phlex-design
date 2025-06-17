Wire-cell Charged Waveform Simulation
=====================================

This note describes one possible workflow the Wire-Cell charged waveform simulation.
This is an attempt to describe the workflow specified in https://github.com/orgs/Framework-R-D/discussions/3#discussioncomment-12997648 using Phlex terminology.
In this document we have concentrated on the "chunking" described in the first bullet item.
The purpose of this document is to describe the workflow with just enough detail to ensure that the higher order functions (HOFs) described are sufficient to the task.

We begin with the assumption that the simulated energy deposit information (e.g. hits simulated by GEANT4) for a single trigger record will fit into memory at one time, even for the 100s supernova burst (SNB) trigger record. [#f1]_
The workflow we propose is illustrated in :numref:`fig-wirecell-func` and :numref:`fig-wirecell`.
Each diagram shows the workflow as a graph.
:numref:`fig-wirecell-func` shows the function-centric view of the workflow, in which the nodes are functions and the edges show the flow of data between the functions.
:numref:`fig-wirecell` shows the data-centric view of the workflow, in which the nodes are data and the edges are relations between the data, including the functions that create the data.
Because the processing we show does not need to span (or, equivalently, *fold over*) multiple trigger records, we show only one trigger record in the diagrams.

.. graphviz:: wirecell-charge-waveform-sim-func.gv
   :caption: A possible Wire-Cell charged waveform simulation workflow in the function-centric view.
             The outer box encloses all of the higher order functions (HOFs) that make up the *sim* workflow.
             Colored rectangles denote the HOFs and the user-supplied algorithms used in them.
             The solid arrow shows the data flow from one HOF to the next one in the workflow.
             The labels on the solid arrows denote the data products.
             The subscripts on the data product names show the index metadata that associate the product with the correct data product sets.
   :name: fig-wirecell-func
   :align: center

.. graphviz:: wirecell-charge-waveform-sim.gv
   :caption: A possible Wire-Cell charged waveform simulation workflow in the data-centric view.
             Colored rectangles denote data product sets.
             Solid lines without an arrow show the relationship between hierarchical data product sets.
             Colored rounded rectangles denote data products.
             The label of the rounded rectangle show the programming language data type of the data product, the index metadata that associate the product with the correct data product sets, and the algorithm that was the creator of the product.
             Different colors represent different hierarchies of data product sets.
             Dotted lines show the data product set to which each data product belongs.
             Unshaded rectangles denote data product sequences; the sequence consists of the data products within the rectangle.
             Solid lines with arrows show a higher order function (HOF) that creates the data product, or the data product sequence, to which the arrow points.
             The label on the line shows both the HOF being used  and the user-supplied algorithm used by that HOF.
   :name: fig-wirecell
   :align: center

Both diagrams use colors to indicate different levels in data product set hierarchies.
There are three hierarchies represented.
Each has *TriggerRecord* as the top level.
The other three data product sets (*TimeBin*, *DriftBin*, and *ConvolutionBin*) are each in a different hierarchy, below *TriggerRecord*.

The *sim* workflow (comprising the *drift*, *convolve*, *noise* and *digitize* steps) begins with the simulated *Depos* for a trigger record.
There is one such *Depos* object in each *TriggerRecord*; these objects is indicated by an index of the form *ti*  indicating the *it* *TriggerRecord*.
This *Depos* object is also associated with its "creator" algorithm, here called *Geant*.



The *Depos* object is the input to an algorithm that deals with the drift of the charge, creating *DriftedDepos* information.
It is the assumption of this analysis that the generated information from this algorithm is too large to hold in memory at one time, so an *unfold* higher order function (HOF) is used to create a sequence of *DriftedDepos* objects, one for each *time bin* that the unfold creates.
This unfold defines both the time bins themselves, and the *DriftedDepos* object that is put into each of the generated time bins.
The period of time covered by each time bin, and the number of time bins, and the contents of the *DriftedDepos* object put into each time bin, are under the control of the user-specified *drift* algorithm that is given to the unfold function.

Each of the *DriftedDepos* objects is then passed as input to another HOF, in this case a *transform*.
The transform applies the user-supplied *convolve* algorithm to each of the *DriftedDepos* objects in the *DriftedDepos* sequence, yielding a *ConvolvedDepos* object for each time bin.

Next, another transform is applied to the *ConvolvedDepos* sequence, in this case applying a user-supplied *noise* algorithm, yielding a *NoisyConvolvedDepos* object for each time bin.

Finally, a *fold* HOF is applied to the *NoisyConvolvedDepos* data product sequence corresponding to each trigger record, to create a single *DigitizedWaveform* object associated with the trigger record.
This fold employs a user-written *digitize* algorithm, which is presented with one *NoisyConvolvedDepos* object, and a *DigitizedWaveform* object to be updated, each time it is called.
The *digitize* algorithm is called one for each *NoisyConvolvedDepos* object in the sequence.
When the fold is complete, the then-finalized *DigitizedWaveform* object is put into the trigger record, where it is available to downstream algorithms.

.. rubric:: Footnotes

.. [#f1] If, for the SNB trigger records, the data are too large to fit into memory, then we would have to start from pre-existing time-binned *Depos* objects, and the initial unfold in this workflow would not be needed.
         If this is true for simulated spill trigger records, but not for simulation SNB trigger records, then either the two cases would use slightly different workflows, or the spill trigger records could just contain a single time bin for spill trigger records.

