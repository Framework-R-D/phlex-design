all: requirements-statemachine.png requirements-lifecycle.pdf

requirements-statemachine.png: requirements-statemachine.gv
	dot -Tpng $< -o $@

requirements-lifecycle.pdf: requirements-lifecycle.md
	pandoc -s -Tpdf  $< -o $@
