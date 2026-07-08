## How Data Products Are Labeled

1. The main data type for the labeling of data products is `product_specification`
2. The type `product_specification` may not actually hold *all* of the data needed to label a data product.
3. FORM storage of data products must ensure that the full metadata must be associated with each product.
4. The [FORM IO interface](form-io-interface.md) must allow finding and reading a product by providing a `product_selector` that matches some fraction of these metadata.
    Form must be able to either read the associated data product or to report that the specification is ambiguous.
    If the specification is ambiguous the matching candidates should be identified and the returned error message should include the metadata of those candidates.

### The Data Required for Complete Product Specification

Complete product specification means that this information is sufficient to uniquely identify a single data product.

1. creator (also called node name) (plugin_name + algorithm_name)
2. suffix
3. type_id (the concrete type id)
4. stage_name
5. a set of data cell indices identify which data-cells the product is associated with
6. a data-product concept
