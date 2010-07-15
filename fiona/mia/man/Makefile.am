# the help option that should be used in the manpage
HELPOPTION = --full-help


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################
man_MANS = $(PACKAGE).1
MAINTAINERCLEANFILES = $(man_MANS) $(PACKAGE).pdf
EXTRA_DIST = $(man_MANS) extra

$(PACKAGE).1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo extra
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/$(PACKAGE) \
		--name="$(PACKAGE_NAME)" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="$(HELPOPTION)" \
		--opt-include=extra

$(PACKAGE).pdf:
	$(AM_V_GEN)groff -Tps -mandoc -c $(PACKAGE).1 | ps2pdf - $(PACKAGE).pdf

man-pdf: $(PACKAGE).1 $(PACKAGE).pdf
