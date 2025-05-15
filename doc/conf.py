# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import subprocess
import sys

sys.path.insert(0, os.path.abspath('.') + '/_extensions')

project = 'Phlex framework'
copyright = '2025, Fermi Forward Discovery Group, LLC'
author = ''

git_commit = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

version = "0"
release = f"0 ({git_commit})"

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
\usepackage{xparse}
\usepackage{etoolbox}
"""

preliminary_watermark = r"""
\usepackage[angle=30,color=lightgray]{background}
\backgroundsetup{contents={Preliminary}}"""

new_commands = r"""
\NewDocumentCommand{\iset}{g}{
  \IfNoValueTF{#1}
    {\mathcal{I}}
    {\mathcal{I}_#1}
}

\newcommand\comp[0]{\circ}

\NewDocumentCommand{\sequence}{mg}{
  \IfNoValueTF{#2}
    {[#1_i]_{i \in \iset}}
    {[#1_i]_{i \in \iset{#2}}}
}

\newcommand\transform[1]{\text{transform}(#1)}
\newcommand\predicate[1]{\text{predicate}(#1)}
\newcommand\filter[1]{\text{filter}(#1)}
\newcommand\observe[1]{\text{observe}(#1)}
\newcommand\unfold[3]{\text{unfold}(#1,\ #2,\ #3)}

\NewDocumentCommand{\fold}{mmg}{
  \IfNoValueTF{#3}
    {\text{fold}(#1,\ #2)}
    {\text{fold}(#1,\ #2,\ #3)}
}

% The following is needed to ensure consistent footnote ordering
% within each LaTeX-rendered chapter:
%  (see https://github.com/sphinx-doc/sphinx/issues/3652#issuecomment-303195583)
\makeatletter
\def\FNH@footnoteenv@i[#1]{\FNH@footnoteenv}
\def\FNH@footnotetextenv@i[#1]{\FNH@footnotetextenv}
\def\sphinxfootnotemark [#1]%
   {\ifx\thepage\relax\else\protect\spx@opt@BeforeFootnote
                             \protect\footnotemark\fi}%
\makeatother

% Remove extra vertical space after line blocks
\AtEndEnvironment{DUlineblock}{\vspace*{-1.5\baselineskip}}

% Number subsubsections
\setcounter{secnumdepth}{3}
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
    "preamble": extra_packages + preliminary_watermark + new_commands,
}
latex_documents = [("index",
                    f"phlex-design-v{version}-{git_commit}.tex",
                    project,
                    author,
                    "manual",
                    False)]
