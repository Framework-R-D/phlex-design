# Phlex design
Design ideas for Phlex, which is intended to satisfy DUNE's data-processing framework requirements.

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
    - If you do not already have all the necessary software installed, you might install [Conda](https://conda-forge.org/download/), and use Conda to install the required products. Full documentation for Conda is available at https://docs.conda.io/en/latest/.  Assuming you already have a LaTeX installation, the following Conda command will install everything else you need into an environment named `for-dune-framework-docs`.
```
        conda create -n for-dune-framework-docs sphinx plantuml doxygen pandoc sphinx-needs
```

3. Navigate to the `doc` subdirectory and type `make latexpdf`.
