import re
from docutils import nodes
from sphinx.addnodes import pending_xref

def setup(app):
    requirement_pattern = re.compile(r"([\d\.]+)\s+(.+)")
    app.add_role("dune", autoref("DUNE", requirement_pattern))

def autoref(experiment, pattern):
    def role(name, rawtext, text, lineno, inliner, options=None, content=None):
        match = pattern.match(text)
        if not match:
            msg = inliner.reporter.error(
                f"Malformed input '{text}'. Expected format: '<number> <requirement description>'",
                line=lineno,
            )
            return [], [match]

        number, description = match.groups()
        ref_target = f"requirements:{number} {description.lower()}"

        # Create pending_xref node (resolved internally by Sphinx)
        node = pending_xref(
            '',
            nodes.Text(f"[{experiment} {number}]"),
            refdomain='std',
            reftype='ref',
            reftarget=ref_target,
            refexplicit=True
        )
        return [node], []

    return role
