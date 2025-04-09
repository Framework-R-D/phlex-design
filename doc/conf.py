# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import sys

sys.path.insert(0, os.path.abspath('.') + '/_extensions')

project = 'Phlex framework'
copyright = '2025, Fermi Forward Discovery Group, LLC'
author = 'Phlex developers'

version = "0"
release = "0.1"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['sphinx.ext.graphviz',
              'sphinx.ext.autosectionlabel',
              'requirement']

autosectionlabel_prefix_document = True
numfig = True
numfig_format = {
    'code-block': 'Listing %s',
    'figure': 'Fig. %s',
    'section': 'Section %s',
    'table': 'Table %s',
}

templates_path = ['_templates']

extensions.append('sphinx.ext.todo')
todo_include_todos = True
todo_emit_warnings = True

# -- Rendering LaTeX math expressions-----------------------------------------

# To create both LaTeX and HTML documentation, it is simplest for math expressions
# to be rendered using LaTeX and then used as images in the HTML.  This is done using
# the sphinx.ext.imgmath extension.
#
# N.B. To the extent possible, multiline equations SHOULD NOT be used in tables.  Sphinx's
#      rST-to-LaTeX conversion introduces the amsmath 'split' environment, which does not
#      work well with the table environments.
#
#      If necessary, you can use the :no-wrap: math option
#      (https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html#directive-option-math-no-wrap)

extra_packages = r"""
\usepackage{bbm}
\usepackage{relsize}
"""

new_commands = r"""
\newcommand\comp[0]{\circ}
\newcommand\sequence[1]{{(#1_i)_{i \in \mathcal{I}}}}
\newcommand\transform[2]{\text{transform}\{#1, #2\}}
\newcommand\fold[3]{\text{fold}\{#1, #2, #3\}}
"""

extensions.append('sphinx.ext.imgmath')
imgmath_image_format='svg'
imgmath_font_size = 13
imgmath_latex_preamble=extra_packages + r"\usepackage{fouriernc}" + new_commands

# -- Code-highlighting roles -------------------------------------------------

rst_prolog ="""
.. role:: cpp(code)
   :language: c++
   :class: highlight

.. role:: py(code)
   :language: py
   :class: highlight
"""

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'bizstyle'
html_logo = 'phlex-logo.svg'
html_static_path = ['_static']
html_style='phlexframework.css'


# -- Options for LaTeX output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-latex-output

latex_logo = 'phlex-logo.png'
latex_elements = {
    "preamble": extra_packages + new_commands,
}
