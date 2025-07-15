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
             The subscripts on the data product names show the index metadata that associate the product with the correct data cells.
   :name: fig-wirecell-func
   :align: center

.. graphviz:: wirecell-charge-waveform-sim.gv
   :caption: A possible Wire-Cell charged waveform simulation workflow in the data-centric view.
             Colored rectangles denote data cells.
             Solid lines without an arrow show the relationship between hierarchical data cells.
             Colored rounded rectangles denote data products.
             The label of the rounded rectangle show the programming language data type of the data product, the index metadata that associate the product with the correct data cells, and the algorithm that was the creator of the product.
             Different colors represent different hierarchies of data cells.
             Dotted lines show the data cell to which each data product belongs.
             Unshaded rectangles denote data product sequences; the sequence consists of the data products within the rectangle.
             Solid lines with arrows show a higher order function (HOF) that creates the data product, or the data product sequence, to which the arrow points.
             The label on the line shows both the HOF being used  and the user-supplied algorithm used by that HOF.
   :name: fig-wirecell
   :align: center

Both diagrams use colors to indicate different levels in the data layer hierarchy.
The top level of the hierarchy is the *TriggerRecord*.
Each of the other layers below the *TriggerRecord* is indicated by a different color.
The three created data cells (*TimeBin*, *DriftBin*, and *ConvolutionBin*) are each in a different layer, below *TriggerRecord*.

The *sim* workflow (comprising the *drift*, *convolve*, *noise* and *digitize* steps) begins with the simulated *Depos* for a trigger record.
There is one such *Depos* object in each *TriggerRecord*; this object is indicated by an index of the form *ti*  indicating the *ith* *TriggerRecord*.
This *Depos* object is also associated with its metadata that indicates which algorithm created it, in this case an algorithm called *Geant*.

This *Depos* object is passed as input to the *unfold* higher order function (HOF).
This HOF employs a user-written *timebin* algorithm to make smaller *Depos* objects (chunks), one in each time bin.
The output of the *unfold* is a sequence of *Depos* objects, each labeled with a two-part index, *ti* and *bj*, which indicates an association with the *ith* *TriggerRecord* and with the *jth* time bin in that *TriggerRecord*.
This sequence is labeled "Timebinned Depos".
The *Depos* object in this sequence is also associated with its metadata that indicates which algorithm created them, in this case an algorithm called *timebin*.

The time-binned *Depos* sequence is then passed to another HOF, in this case a *window* function.
The *window* function applies a user-supplied *drift* algorithm to each consecutive pair of *Depos*  objects in the time-binned *Depos* sequence, yielding a new *Depos* object.
Each pair of consecutive time bins specifies a new *drift bin*, each of which contains a single (drifted) *Depos* object.
Note that each *drift bin* represents some span of time, but a different one than is represented by the *time bins* created by the earlier *unfold*.
The output of the *window* is a sequence of *Depos* objects, each labeled with a two-part index, *ti* and *dj*, which indicates an association with the *ith* *TriggerRecord* and with the *jth* drift bin in that *TriggerRecord*.
This new sequence is labeled "Drifted Depos".
The *Depos* object in this sequence is also associated with its metadata that indicates which algorithm created it, in this case an algorithm called *drift*.

The drifted *Depos* sequence is then passed to another HOF, in this case a *transform*.
The *transform* applies a user-supplied *convolve* algorithm to each *Depos* object in the drifted *Depos* sequence, yielding a new *ConvolvedDepos* object for each time bin.
Each *ConvolvedDepos* object is associated with its metadata that indicates which algorithm created it, in this case an algorithm called *convolve*.
They are labeled with the same two-part index, *ti* and *dj*, which indicates an association with the *ith* *TriggerRecord* and with the *jth* drift bin in that *TriggerRecord*.
The new sequence is labeled "Convolved Depos".
The *ConvolvedDepos* object in this sequence is also associated with its metadata that indicates which algorithm created it, in this case an algorithm called *convolve*.
The *ConvolvedDepos* objects in this sequence need adjustment to correctly take into account some "leakage" between adjacent (in time) *ConvolvedDepos* objects.

This correction is done by the next step.
The convolved *Depos* sequence is passed to the next HOF, which is another *window*.
This *window* function applies a user-supplied *normalize* algorithm to each consecutive pair of *ConvolvedDepos* objects in the convolved *Depos* sequence, yielding a single new, leakage-corrected *ConvolvedDepos* object contained by a new convolution bin.
Note that each *convolution bin* also represents some span of time, but a different one than is represented by the *time bins* or *drift bins*  created by the earlier *unfolds*.
The output of the *window* is a sequence of *ConvolvedDepos* objects, each labeled with a two-part index, *ti* and *cj*, which indicates an association with the *ith* *TriggerRecord* and with the *jth* convolution bin in that *TriggerRecord*.
This new sequence is labeled "Corrected ConvolvedDepos".
The *ConvolvedDepos* object in this sequence is also associated with its metadata that indicates which algorithm created it, in this case an algorithm called *normalize*.


The corrected *ConvolvedDepos* sequence is then passed to another HOF, in this case a *transform* function.
The *transform* applies a user-supplied *noise* algorithm to each *ConvolvedDepos* object in the corrected *ConvolvedDepos* sequence, yielding a new *NoisyConvolvedDepos* object for each convolution bin.
Each *NoisyConvolvedDepos* object is associated with its metadata that indicates which algorithm created it, in this case an algorithm called *noise*.
They are labeled with the same two-part index, *ti* and *cj*, which indicates an association with the *ith* *TriggerRecord* and with the *jth* convolution bin in that *TriggerRecord*.
The new sequence is labeled "Noisy ConvolvedDepos".
The *NoisyConvolvedDepos* object in this sequence is also associated with its metadata that indicates which algorithm created it, in this case an algorithm called *noise*.

The final step of the workflow is to apply a *fold* higher order function (HOF) to the *NoisyConvolvedDepos* sequence.
This *fold* employs a user-written *digitize* algorithm, which produces a single *DigitizedWaveform* object for each trigger record.
There is one such *DigitizedWaveform* object in each *TriggerRecord*; this object is indicated by an index of the form *ti*  indicating the *ith* *TriggerRecord*.

.. rubric:: Footnotes

.. [#f1] If, for the SNB trigger records, the data are too large to fit into memory, then we would have to start from pre-existing time-binned *Depos* objects, and the initial unfold in this workflow would not be needed.
         If this is true for simulated spill trigger records, but not for simulation SNB trigger records, then either the two cases would use slightly different workflows, or the spill trigger records could just contain a single time bin for spill trigger records.

