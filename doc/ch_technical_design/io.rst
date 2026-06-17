IO
==

.. todo::

   IO includes output and the input of all data not provided in a configuration file or on the command-line:

   - This could also include external database use cases.
   - This would also include the data-handling mechanisms.
   - This may include a textual output, which may or may not be parse-able for input
 
FORM integration and IO-backed provider synthesis
-------------------------------------------------

1. Phlex will support reading one FORM file as input.
2. Phlex will enable registration of providers that return type-erased data.
   Type-erased data in this context means that the data are returned as base class type, not seen by the user, but known to Phlex and FORM.
3. After opening an input file, FORM will provide metadata used by the framework to synthesize a provider node for each data product required by the workflow.
4. `std::type_info` will be used to request from FORM a data product with a specific type.
5. An unrequited data product will have a provider created for it.
6. An exception will be raised if more than one data product in the FORM file matches the specified product query, including its `std::type_info`.
7. Phlex or FORM must be able to generate a reproducible name for a type if no product suffix is associated with the data product.
8. The data-product concept must be persisted in some form to a file through Phlex's IO backend interface, such as FORM.

It is possible that the use of `std::type_info` may be problematic across process boundaries.

