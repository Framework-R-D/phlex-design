Data Modeling
=============

.. todo::

   - Provides the mechanism by which users define data product types.
   - Provides a mechanism for connecting in-memory data representations with information needed by the IO backends to read-and-write the data
   - Responsible for ensuring the same data product is representable in the supported languages
   - May need to translate from one in-memory representation to another when crossing a language boundary


A data-product concept can be modeled by multiple concrete data-product types.
A concrete data-product type may model more than one data-product concept.

The specification of input data products must support data-product concepts.
This may include an optional explicit specification of the data-product concept.
