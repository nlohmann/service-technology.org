man_MANS = petri.1

MAINTAINERCLEANFILES = $(man_MANS)

EXTRA_DIST = $(man_MANS)

petri.1: $(top_srcdir)/configure.ac
	$(HELP2MAN) \
		$(top_builddir)/utils/petri \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help"
