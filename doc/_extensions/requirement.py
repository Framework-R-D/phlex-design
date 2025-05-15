import re

from docutils import nodes
from sphinx.addnodes import pending_xref


def setup(app):
    requirement_pattern = re.compile(r"([\d\.]+)\s+(.+)")
    app.add_role("dune", autoref("DUNE", requirement_pattern))
    return {'version': '1.0', 'parallel_read_safe': True}

def autoref(experiment, pattern):
    def role(name, rawtext, text, lineno, inliner, options=None, content=None):
        # Support multiple specifications in the same role invocation
        all_specs = text.split(",")
        matches = []
        failed = []
        for spec in all_specs:
            match = pattern.match(spec.strip())
            if match:
                matches.append(match)
            else:
                msg = inliner.reporter.error(
                    f"Malformed input '{spec}'. Expected format: '<number> <requirement description>'",
                    line=lineno,
                )
                failed.append(match)

        if failed:
            return [], failed

        new_nodes = []
        size = len(matches)
        for i, match in enumerate(matches, start=1):
            number, description = match.groups()
            ref_target = f"requirements:{number} {description.lower()}"

            # Create pending_xref node (resolved internally by Sphinx)
            prefix = "[" if i == 1 else " "
            suffix = "]" if i == size else ","
            new_nodes.append(pending_xref(
                '',
                nodes.Text(f"{prefix}{experiment} {number}{suffix}"),
                refdomain='std',
                reftype='ref',
                reftarget=ref_target,
                refexplicit=True
            ))
        return new_nodes, []

    return role
