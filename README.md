# dune-framework-design
Design ideas for the DUNE data-processing framework

## Building the design document

The design document is assembled using [Sphinx](https://www.sphinx-doc.org/en/master/index.html).
To build the document, clone this repository. You must then have:

- An installation of [Sphinx](https://www.sphinx-doc.org/en/master/usage/installation.html)
- `pdflatex` installed along with the required LaTeX packages
  - Most TeX installations will include the required packages.
    If additional packages are required, a LaTeX error will be generated when typing `make latexpdf`, and the required packages can be installed using `tlmgr`.
- An installation of [Graphviz](https://graphviz.org/download/)

Once these have been installed, navigate to the `doc` subdirectory and type `make latexpdf`.
