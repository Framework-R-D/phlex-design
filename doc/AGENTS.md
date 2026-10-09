## Building the docs

Sphinx project under `doc/` (`doc/Makefile`, `conf.py`); Quarto `.qmd` files and
`_quarto.yml` at root; `GNUmakefile` drives top-level builds. `rstcheck` config
in `.rstcheck.cfg`. Respect markdownlint rules (MD012/MD022/MD031/MD032/MD034/
MD040) and the global file-formatting rules (one trailing newline, no trailing
whitespace).
