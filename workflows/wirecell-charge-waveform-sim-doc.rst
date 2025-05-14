Wire-cell Charged Waveform Simulation
=====================================

This note describes one possible workflow the Wire-Cell charged waveform simulation.

We begin with the assumption that the simulated energy deposit information (e.g. hits simulated by GEANT4) for a single trigger record will fit into memory at one time, even for the 100s supernova burst (SNB) trigger record. [#f1]_
:numref:`fig-wirecell` illustrates the workflow.
Because the processing we show does not need to span (or, equivalently, *fold over*) multiple trigger records, we show only one trigger record in the diagram.

.. graphviz:: wirecell-charge-waveform-sim.gv
   :caption: A possible Wire-Cell charged waveform simulation workflow.
             Shaded rectangles denote data product sets.
             Solid lines without an arrow show the relationship between hierarchical data product sets.
             Shaded rounded rectangles denote data products.
             Dotted lines show the data product set to which each data product belongs.
             Unshaded rectagles denote data product sequences; the sequence consists of the data products within the rectangle.
             Solid lines with arrows show an algorithm (a higher order function) that creates the data product, or the data product sequence, to which the arrow points.
             The label on the line shows both the higher order function being used  and the user-supplied algortithm used by that higher order function.
   :name: fig-wirecell

The workflow begins with the simulated *Deposits* for a trigger record.
The *Deposits* object is to be used as input to an algorithm that deals with the drift of the change to the wires.
The generated information from this algorithm is too large to hold in memory at one time, so an *unfold* higher order function is used to create a sequence of *DriftedDepos* objects, one for each *time bin* that the higher order function defines.
This unfold defines both the time bins themselves, and the *DriftedDepos* object that is put into each of the generated time bins.



.. rubric:: Footnotes

.. [#f1] If, for the SNB trigger records, the data are too large to fit into memory, then we would have to start from pre-existing time-binned Depos objects, and the initial unfold in this workflow would not be needed.
         If this is true for simulated spill trigger records, but not for simulation SNB trigger records, then either the two cases would use slightly different workflows, or the spill trigger records could just contain a single time bin for spill trigger records.



