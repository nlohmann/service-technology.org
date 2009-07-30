man_MANS = petri.1

MAINTAINERCLEANFILES = $(man_MANS) petri.pdf

EXTRA_DIST = $(man_MANS) extra

petri.1: $(top_srcdir)/configure.ac $(top_srcdir)/utils/cmdline.ggo extra
	$(HELP2MAN) \
		$(top_builddir)/utils/petri \
		--name="$(PACKAGE_NAME) Frontend" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--full-help" \
		--opt-include=extra \
		--no-info

man-pdf: petri.1
	groff -Tps -mandoc -c petri.1 | ps2pdf - petri.pdf
