Data-Product Providers
======================

Data-product providers are framework components that provide data products to downstream configured HOFs.
They take as input data cell indices.
A user-supplied provider invokes a user-supplied *provider algorithm*.
There are also framework-supplied providers that use framework IO subsystem to read data products from framework supported data files.
These providers communicate with the IO system through a specified API that is implemented by each IO back end.
Providers communicate with the IO system only through this API.
This allows the framework to support multiple IO back ends, including ROOT :need:`DUNE 74` and HDF5 :need:`DUNE 141`, and ensure that new back ends can be added without modifying the framework code :need:`DUNE 73`.

Providers are responsible for reading data written by earlier code versions, subject to policy decisions made by the experiment :need:`DUNE 76`.

Providers are also responsible for reading some types of data (such as calibration data, or geometry descriptions) from sources other than files written by the IO system.
The workflow shown in :numref:`workflow` shows an example of one such *provider* algorithm, which reads geometry data from a GDML file.

Provider Function Signature
---------------------------

The provider function take a data cell index as input and returns a data product.

.. code-block:: cpp

   return_type function_name(data_cell_index const& id)

The :cpp:`reurn_type` must model the created data-product type described in :numref:`ch_conceptual_design/algorithms:Return Types`.
Only a single data product can be returned by a provider function.


Provider Registration
---------------------
To illustrate the how a provider function is registered with Phlex, we use the following class and function, which are presumably defined in some experiment libraries.

.. code:: c++

   class SimDepos { ... };

   SimDepos provide_sim_depos(data_cell_index const&) { ... }

**Provider function that returns SimDepos**

.. code-block:: cpp

   PHLEX_REGISTER_PROVIDERS(s)
   {
     s.provide("fake_sim_depos", provide_sim_depos, concurrency::unlimited)
       .output_product_suffixes("");
   }

**Provider registration**

This registers a provider that inserts a :product:`SimDepos` data product into each data cell in the data layer indicated by the data cell index.
In this example, the data product suffix provided to :cpp:`output_product_suffixes` is an empty string.
The same affect can be achieved by omitting the :cpp:`output_product_suffixes` call entirely.
