Data-Product Writers
====================

Writers write both (user-defined) data products and metadata, both system- and user-defined, to persistent output :need:`DUNE 17`.
Files written to by writers can be read using providers, through the same IO back end :need:`DUNE 19`.
The metadata written by writers is sufficient to allow the reproduction of the processing that created the written data :need:`DUNE 28`.
This means means that, for example, the full configuration of the framework executable is included in the written metdata.

Writers use the same IO API as providers, ensuring that the only communication with the IO system is through the IO API.
This allows the framework to support multiple IO back ends, including ROOT :need:`DUNE 74` and HDF5 :need:`DUNE 141`, and ensure that new back ends can be added without modifying the framework code :need:`DUNE 73`.
The workflow shown in :numref:`workflow` shows an example with the *write* algorithm writing data products to a ROOT back end.
