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

project = 'Phlex'
copyright = '2025, Fermi Forward Discovery Group, LLC'
author = ''

git_commit = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

version = "0"

# For releases for circulation use:
release = "0.2"
latex_filename = f"phlex-design-v{release}.tex"

# For development use
# release_base = "0.2.alpha"
# latex_filename = f"phlex-design-v{release_base}-{git_commit}.tex"
# release = f"{release_base} ({git_commit})"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['sphinx.ext.graphviz',
              'sphinx.ext.autosectionlabel',
              'appendix',
              "missing_references",
              "sphinx_needs",
              "terminology"]

# Unfortunately, using 'svg' as the graphviz output format creates oddly placed subscripts in the graphs.
# graphviz_output_format = "svg"
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

needs_id_regex = r"^DUNE\s+\d+"
needs_role_need_template = "[{id}]"
needs_extra_options = ["jama", "see", "notes"]
needs_string_links = {
    # Adds link to the Sphinx-Needs configuration page
    'jama_link': {
        'regex': r'^(?P<number>\d+)\s+(?P<jamaurl>.*)$',
        'link_url': '{{ jamaurl }}',
        'link_name': 'DUNE-DUNE_STKH-{{ number }}',
        'options': ['jama']
    },
}

# -- Rendering LaTeX math expressions-----------------------------------------

# Math expressions must be renderable in both LaTeX and HTML.  To do this, we define
# TeX-like macros in both LaTeX and MathJax (which is used for HTML rendering).
# The LaTeX macros are defined in the latex_elements['preamble'] section, while the
# MathJax macros are defined in the mathjax3_config dictionary.  The macros are
# defined in a way that they can be used in both LaTeX and MathJax, so that the
# same source can be used for both PDF and HTML rendering.
#
# N.B. To the extent possible, multiline equations SHOULD NOT be used in tables.  Sphinx's
#      rST-to-LaTeX conversion introduces the amsmath 'split' environment, which does not
#      work well with the table environments.
#
#      If necessary, you can use the :no-wrap: math option
#      (https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html#directive-option-math-no-wrap)

extra_packages = r"""
\usepackage{bbm}
\usepackage{mathtools}
"""

# "For DUNE Review", use scale=7 for watermark
# "For Review", use scale=10.5 for watermark

preliminary_watermark = r"""
\usepackage[angle=30,color=lightgray]{background}
\backgroundsetup{scale=10.5, contents={For Review}}"""

new_commands = r"""
\newcommand\lt[0]{<}
\newcommand\bool[0]{\mathbb{B}}
\newcommand\one[0]{\mathbbm{1}}
\newcommand\opt[1]{\text{Opt}(#1)}
\newcommand\isetdefault[0]{\mathcal{I}}
\newcommand\iset[1]{\isetdefault_{#1}}
\newcommand\comp[0]{\circ}

\newcommand\fami[1]{\left[#1_i\right]}
\newcommand\family[1]{\left[#1_i\right]_{i \in \isetdefault}}
\newcommand\ifamily[2]{\left[#1_i\right]_{i \in \iset{#2}}}
\newcommand\fold[2]{\text{fold}(#1,\ #2)}
\newcommand\pfold[3]{\text{fold}(#1,\ #2,\ #3)}

\newcommand\transform[1]{\text{transform}(#1)}
\newcommand\predicate[1]{\text{predicate}(#1)}
\newcommand\filter[1]{\text{filter}(#1)}
\newcommand\observe[1]{\text{observe}(#1)}
\newcommand\unfold[2]{\text{unfold}(#1,\ #2)}
\newcommand\punfold[3]{\text{unfold}(#1,\ #2,\ #3)}
\newcommand\window[3]{\text{window}(#1,\ #2,\ #3)}

% The following is needed to ensure consistent footnote ordering
% within each LaTeX-rendered chapter:
%  (see https://github.com/sphinx-doc/sphinx/issues/3652#issuecomment-303195583)
% and then to never reset the counter at the chapter level
%  (https://texfaq.org/FAQ-run-fn-nos)
\makeatletter
\def\FNH@footnoteenv@i[#1]{\FNH@footnoteenv}
\def\FNH@footnotetextenv@i[#1]{\FNH@footnotetextenv}
\def\sphinxfootnotemark [#1]%
   {\ifx\thepage\relax\else\protect\spx@opt@BeforeFootnote
                             \protect\footnotemark\fi}%
\makeatother
\counterwithout*{footnote}{chapter}

% Number subsubsections
\setcounter{secnumdepth}{3}

% Make table caption width span entire textwidth
\usepackage[width=\textwidth]{caption}
"""

extensions.append('sphinx.ext.mathjax')
mathjax3_config = {
  "loader": {"load": ['[tex]/newcommand', '[tex]/textmacros', '[tex]/mathtools']},
  "tex": {
      "packages": {'[+]': ['newcommand', 'textmacros', 'mathtools']},
      "macros": {
          "bool": ['\\mathbb{B}', 0],
          "one": ['\\Bbb{1}', 0],
          "opt": ['\\text{Opt}(#1)', 1],
          "transform": ['\\textrm{transform}(#1)', 1],
          "predicate": ['\\textrm{predicate}(#1)', 1],
          "filter": ['\\textrm{filter}(#1)', 1],
          "observe": ['\\textrm{observe}(#1)', 1],
          "unfold": ['\\textrm{unfold}(#1, #2)', 2],
          "punfold": ['\\textrm{unfold}(#1, #2, #3)', 3],
          "fold": ['\\textrm{fold}(#1, #2)', 2],
          "pfold": ['\\textrm{fold}(#1, #2, #3)', 3],
          "window": ['\\textrm{window}(#1, #2, #3)', 3],
          "isetdefault": ['\\mathcal{I}', 0],
          "iset": ['\\isetdefault_{#1}', 1],
          "fami": ['\\left[#1_i\\right]', 1],
          "family": ['\\left[#1_i\\right]_{i \\in \\isetdefault}', 1],
          "ifamily": ['\\left[#1_i\\right]_{i \\in \\iset{#2}}', 2],
          "comp": ['\\circ', 0],
      }
      }
}

# -- Code-highlighting roles -------------------------------------------------

rst_prolog ="""
.. role:: cpp(code)
   :language: c++
   :class: highlight

.. role:: py(code)
   :language: py
   :class: highlight

.. role:: haskell(code)
   :language: haskell
   :class: highlight
"""

# -- When we only want the conceptual design
if conceptual_design_only := os.environ.get("PHLEX_CONCEPTUAL_DESIGN_ONLY", False):
    tags.add("conceptual_design_only")  # pyright: ignore[reportUndefinedVariable]
    exclude_patterns = ["supporting_design.rst",
                        "ch_subsystem_design/*",
                        "appendices/deployment.rst",
                        "appendices/platforms.rst"]
    suppress_warnings = ["toc.excluded"]

# -- Detect if PDF rendering is available and provide link to it in HTML rendering (see index.rst)
if existing_pdf := os.environ.get("SPHINX_EXISTING_PDF"):
    tags.add("pdf_already_exists")  # pyright: ignore[reportUndefinedVariable]
    rst_epilog = f".. _here: {existing_pdf}"
else:
    rst_epilog = ".. _here: here"  # So Sphinx will stop complaining that "here" is not defined

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'bizstyle'
html_logo = 'phlex-logo.svg'
html_static_path = ['_static']
html_style ='phlexframework.css'
html_last_updated_fmt = ""  # Equivalent to showing (e.g.) May 29, 2025


# -- Options for LaTeX output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-latex-output

latex_logo = 'phlex-logo.png'
latex_elements = {
    "preamble": extra_packages + preliminary_watermark + new_commands,
}
latex_documents = [("index",
                    latex_filename,
                    project,
                    author,
                    "manual",
                    False)]
