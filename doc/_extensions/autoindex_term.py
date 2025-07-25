from docutils import nodes
from sphinx.addnodes import index
from sphinx.application import Sphinx

import re

# The refuri has a pattern like (for LaTeX, and similar for HTML)
#   %appendices/definitions#term-Algorithm
# We want the term that follows the string literal "#term-"
index_refuri_pattern = re.compile(r".*#term-(.*)")

# Global lookup from refuri to dictionary term
refuri_to_term = {}


def populate_refuri_mapping(app: Sphinx):
    std_domain = app.env.domains['std']
    objects = std_domain.data['objects']
    for (objtype, term), v in objects.items():
        if objtype != "term":
            continue

        refuri = v[1].removeprefix("term-")
        refuri_to_term[refuri] = term
    return refuri_to_term


def process_term_nodes(app: Sphinx, doctree: nodes.document, fromdocname: str) -> None:
    """
    For each `:term:` role in the document, add a corresponding index entry.
    """
    refuri_to_term = populate_refuri_mapping(app)

    for node in doctree.traverse(nodes.reference):
        # Check if this is a glossary term reference
        if 'refuri' in node and ("#term-" in node["refuri"]):
            refuri_snippet = index_refuri_pattern.match(node["refuri"]).group(1)

            # Inject an index node before this reference
            assert refuri_to_term
            term = refuri_to_term[refuri_snippet]
            index_node = index(entries=[('single', term, fromdocname, node.line or 0, "")])
            node.parent.insert(node.parent.index(node), index_node)


def setup(app: Sphinx):
    app.connect('doctree-resolved', process_term_nodes)
    return {
        'version': '1.0',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
