# dune-framework-design
Design ideas for the DUNE data-processing framework

## Building the design document

The design document is assembled using [Sphinx](https://www.sphinx-doc.org/en/master/index.html).
To build the document: 

1. Clone this repository.

2. Make sure you have installations of the following:

    - [Sphinx](https://www.sphinx-doc.org/en/master/usage/installation.html)
    - `pdflatex` along with the required LaTeX packages
      - Most TeX installations will include the required packages.
      If additional packages are required, a LaTeX error will be generated when typing `make latexpdf`, and the required packages can be installed using `tlmgr`.
    - [Graphviz](https://graphviz.org/download/)

3. Navigate to the `doc` subdirectory and type `make latexpdf`.
