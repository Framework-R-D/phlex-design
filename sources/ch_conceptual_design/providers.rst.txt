Data-Product Providers
======================


Providers communicate with the IO system through a specified API that is implemented by each IO back end.
The IO API provides the ability to read (and write) data to the IO system.
Providers communicate with the IO system only through this API.
This allows the framework to support multiple IO back ends, including ROOT :need:`DUNE 74` and HDF5 :need:`DUNE 141`, and ensure that new back ends can be added without modifying the framework code :need:`DUNE 73`.

Providers are responsible for being able to read data written by earlier code versions, subject to policy decisions made by the experiment :need:`DUNE 76`.

Providers are also responsible for be able to read some types of data (such as calibration data, or geometry descriptions) from sources other than files written by the IO system.
The workflow shown in :numref:`workflow` shows an example of two such *provide* algorithms.
The first one is responsible for reading data `Spill`\ s from the IO system, and the second one is responsible for reading the data from GDML files.
