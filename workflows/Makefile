
# Directories
SOURCEDIR = .
BUILDDIR = _build

# Sphinx commands
SPHINXBUILD = sphinx-build
LATEXPDF = $(SPHINXBUILD) -b latex

# LaTeX compiler
PDFLATEX = pdflatex

# Targets
PDFS = $(BUILDDIR)/latex/wirecell-charge-waveform-sim-doc.pdf

.PHONY: all clean

all: $(PDFS)

# Rule to build LaTeX files with Sphinx
$(BUILDDIR)/latex/%.tex: %.rst
	$(LATEXPDF) -d $(BUILDDIR)/doctrees $(SOURCEDIR) $(BUILDDIR)/latex

# Rule to compile LaTeX to PDF
$(BUILDDIR)/latex/%.pdf: $(BUILDDIR)/latex/%.tex
	cd $(BUILDDIR)/latex && $(PDFLATEX) $*.tex && $(PDFLATEX) $*.tex

clean:
	rm -rf $(BUILDDIR)/*
