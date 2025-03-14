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
              'sphinx.ext.todo',
              'requirement']
autosectionlabel_prefix_document = True

templates_path = ['_templates']
todo_include_todos = True
todo_emit_warnings = True

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'bizstyle'
html_logo = 'phlex-logo.svg'
html_static_path = ['_static']
