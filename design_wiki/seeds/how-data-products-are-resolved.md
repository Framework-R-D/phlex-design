## How Data Products are Resolved
### Product Selectors
- Data products are selected using a `product_selector`
- A `product_selector` *must* specify a data product concept
  - This concept may be automatically determined from the input type required by the CHOF being registered
- A `product_selector` *may* specify zero or more of the following
  - Creator name -- This can match either the plugin name or the algorithm name (or both) of the creating algorithm
  - Suffix
  - Stage name -- This can be "CURRENT" to match whatever the current stage name is
  - Layer name

### Layers
- Each data cell is associated with a layer path, which determines its home in the hierarchy
- The "layer name" is the name of the lowest ("leaf") layer in the path
- Each instance of a data product is placed in a data cell
- Each instance of a data product is therefore transitively associated with a layer path, and layer name
- These do not have to be constant across instances of a data product

### Resolution
- All fields except layer name are used to resolve the data product at graph creation time
  - This is because the layer name may not be constant across instances of the product
- Edges are created only from input products which match all specified fields of the selector
- Where the product is produced by a HOF other than a provider the relevant fields are: Concept, Creator name, and suffix
- Where the product is produced by a provider, the relevant fields are: Concept, creator name, suffix, and layer name
- Where the product is produced by an *explicit* provider, the layer name is also used at graph creation time
  - Explicit providers statically specify the layer path associated with their output

### Layers Checking / Resolution
- It is desirable that a CHOF be able to take a given input product from different layers on different iterations
  - On each iteration the output layer of the CHOF is determined by the layers of its inputs
- Therefore layer name checking must be delayed til runtime, when data flows along the created edges

#### Contentious Part
- A selector may match multiple products when its layer name is ignored, while only matching a single product in any given data cell
- This requires allowing the creation of multiple edges for the same input port
- We must nevertheless prevent duplicate matches within a single data cell
  1. Could require all other fields to match
     - Perhaps places unreasonable burden on user (all other fields must be specified in the query)
     - Reduces utility of multiple match feature, perhaps completely
  2. Could (somehow) statically enforce behaviour as products are placed into cells
  3. Could adopt a "cycle" concept, where multiple matches within a data cell have a priority order
    - Likely to cause surprising behaviour, at least sometimes.
     
