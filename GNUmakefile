# Documents to build using latexmk and friends (automatically added to
# all target).
PRODUCTS = \
  requirements-lifecycle.pdf \
  dune-fw-design-ideas-chg.pdf \
  dune-req-009-issues.pdf \
  dune-fw-design-concerns-chg-slides.pdf \
	chunkable.pdf

# All PDFs should be made portable, with subsetted fonts.
PORTABLE_PDF = 1

# Show the command lines used.
VERBOSE = 5

# Don't show command output or error.
QUIET = 0

####################################
# # Use doc.mk to produce our documents.
ifneq (,$(SSDDOC_INC))
  ddir = $(SSDDOC_INC)
else
  ddir = ../cet-is/ssddoc/include
endif

include $(ddir)/doc.mk

# Check we have the version we need of doc.mk.
verified_doc_mk_version := $(call require_doc_mk_version,0.1.0)
####################################

####################################
# Target-specific variables and additive overrides.

# Change GNUmakefile, rebuild things.
$(PRODUCTS): GNUmakefile

# Set highlight style.
$(PRODUCTS): override PANDOC_EXTRA_OPTS += --highlight-style=pygments

# Identify targets requiring beamer.
$(call targets_with,-slides): override PANDOC_BEAMER = YES

# Required by requirements-lifecycle.pdf
requirements-statemachine.png: requirements-statemachine.gv
	dot -Tpng $< -o $@
####################################

####################################
# Make sure we can find everything we need.
export TEXINPUTS=$(DOC_MK_DIR)web-conf/:$(DOC_MK_DIR)/:
export BSTINPUTS=$(DOC_MK_DIR)web-conf/:
####################################

####################################
# Regenerate and clean up .bbl files.
override LATEXMK_EXTRA_OPTS += -bibtex
