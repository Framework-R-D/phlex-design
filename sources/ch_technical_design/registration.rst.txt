Algorithm Registration
======================

.. todo::

   - Provides the coupling between user-specified code and the framework
   - Remember composite CHOFs


Providers and registration policy
---------------------------------


1. Providers attached to an source will be automatically registered for any data products that cannot be provided by an explicit provider or a computational node.
2. Users must be allowed to indicate a precedence for looking up data products from different stages.
3. Phlex will enable registration of providers that return type-erased data.
