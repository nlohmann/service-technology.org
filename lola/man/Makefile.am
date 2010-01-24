# the manpages to create
man_MANS = $(PACKAGE).1

# additional files
EXTRA_DIST = extra

# the help option that should be used in the manpage
HELPOPTION = --full-help

$(PACKAGE).1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo extra
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/$(PACKAGE) \
		--name="a low-level Petri net analyzer" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="$(HELPOPTION)" \
		--opt-include=extra


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

MAINTAINERCLEANFILES = $(man_MANS) $(man_MANS:.1=.pdf)
EXTRA_DIST += $(man_MANS)

man-pdf: $(man_MANS:.1=.pdf)

%.pdf: %.1
	$(AM_V_GEN)groff -Tps -mandoc -c $< | ps2pdf - $(@:.1=.pdf)
