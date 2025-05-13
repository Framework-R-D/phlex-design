# Find all .gv files in the current directory
GV_FILES := $(wildcard *.gv)

# Create corresponding .pdf file names from .gv files
PDF_FILES := $(GV_FILES:.gv=.pdf)

# Default target: build all PDFs
all: $(PDF_FILES)

# Rule to convert a .gv file to a .pdf file using dot
%.pdf: %.gv
	dot -Tpdf $< -o $@

# Clean up generated PDF files
clean:
	rm -f $(PDF_FILES= $(PDF_FILES)

.PHONY: all clean
