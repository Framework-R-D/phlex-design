Data-Product Preservers
=======================

Preservers perserve both (user-defined) data products and (framework- and user-defined) metadata in persistent output :need:`DUNE 17`.
Files written to by preservers can be read using providers, through the same IO back end :need:`DUNE 19`.
The metadata written by preservers is sufficient to allow the reproduction of the processing that created the written data :need:`DUNE 28`.
This means that, for example, the full configuration of the framework executable is included in the written metdata.

Preservers use the same IO API as providers, ensuring that the only communication with the IO system is through the IO API.
This allows the framework to support multiple IO back ends, including ROOT :need:`DUNE 74` and HDF5 :need:`DUNE 141`, and ensure that new back ends can be added without modifying the framework code :need:`DUNE 73`.
The workflow shown in :numref:`workflow` shows an example with the *preserve* algorithm writing data products to a ROOT back end.
