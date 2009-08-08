man_MANS = $(PACKAGE).1

MAINTAINERCLEANFILES = $(man_MANS) $(PACKAGE).pdf

EXTRA_DIST = $(man_MANS) extra

$(PACKAGE).1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo extra
	$(HELP2MAN) \
		$(top_builddir)/src/$(PACKAGE) \
		--name="$(PACKAGE_NAME)" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--full-help" \
		--opt-include=extra

man-pdf: $(PACKAGE).1
	groff -Tps -mandoc -c $(PACKAGE).1 | ps2pdf - $(PACKAGE).pdf
