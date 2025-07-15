from docutils.parsers.rst import roles
from docutils import nodes

def setup(app):
    roles.register_local_role('product', italicized_math)
    roles.register_local_role('mathfunc', italicized_math)
    return {'version': '1.0', 'parallel_read_safe': True}

def italicized_math(name, rawtext, text, lineno, inliner, options={}, content=[]):
    # Wrap the text however you'd like
    wrapped_text = r'\textit{' + text.replace("_", "\_") + r'}'

    # Create an inline math node
    node = nodes.math(rawtext, wrapped_text)
    return [node], []
