# We would like to be able to build the documentation that includes only the conceptual design.
# To do this, we exclude the subsystem design files from building whenever the PHLEX_CONCEPTUAL_DESIGN_ONLY
# environment variable is specified.
# We then replace missing references to the subsystem design with a phrase
# "the subsystem design (under preparation)"

from docutils import nodes
import os
import glob
import re


def exclude_patterns(app):
    return [re.compile(glob.translate(p.removesuffix(".rst"))) for p in app.config.exclude_patterns]


def missing_reference_handler(app, env, node, contnode):
    # Assume all excluded patterns correspond to the subsystem design
    ex_patterns = exclude_patterns(app)
    filestem = node["reftarget"].split(sep=":")[0]
    matches = any(p.match(filestem) for p in ex_patterns)
    if matches:
        print(f"Replacing '{node['reftarget']}' with placeholder text")
        fallback = nodes.inline()
        fallback += nodes.Text("the subsystem design ")
        fallback += nodes.emphasis(text="(under preparation)")
        return fallback

    # Use Sphinx's normal missing-reference handling if not excluding the subsystem design
    return None

def setup(app):
    if os.environ.get("PHLEX_CONCEPTUAL_DESIGN_ONLY", False):
        app.connect('missing-reference', missing_reference_handler)
