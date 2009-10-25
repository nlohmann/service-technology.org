man_MANS = petri.1

MAINTAINERCLEANFILES = $(man_MANS) petri.pdf

EXTRA_DIST = $(man_MANS) extra

petri.1: $(top_srcdir)/configure.ac $(top_srcdir)/utils/cmdline.ggo extra
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/utils/petri \
		--name="$(PACKAGE_NAME) Frontend" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--full-help" \
		--opt-include=extra \
		--no-info

petri.pdf:
	$(AM_V_GEN)groff -Tps -mandoc -c petri.1 | ps2pdf - petri.pdf

man-pdf: petri.1 petri.pdf
