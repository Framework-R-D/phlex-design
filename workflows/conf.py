project = 'Possible DUNE Workflows'
author = 'Phlex development team'
version = '1.0'

extensions = ['sphinx.ext.graphviz',
              'sphinx.ext.autosectionlabel',
             ]
autosectionlabel_prefix_document = True
numfig = True
numfig_format = {
    'code-block': 'Listing %s',
    'figure': 'Fig. %s',
    'section': 'Section %s',
    'table': 'Table %s',
}


graphviz_output_format = 'png'  # PNG for LaTeX compatibility

latex_elements = {
    'papersize': 'letterpaper',
    'pointsize': '10pt',
    'fontpkg': r'\usepackage{bookman}',
    'preamble': r'''
       \usepackage{microtype}
       \usepackage{caption}
       \captionsetup{font=footnotesize}
       \usepackage{hyperref}
       ''',
}

latex_documents = [
    ('index', 'wirecell-charge-waveform-sim-doc.tex', 'Wire-Cell Charge Waveform Simulation', 'Marc Paterno and Saba Sehrish', 'howto'),
]
