# the manpages to create
man_MANS = petri.1 cdnf.1 configurator.1

# additional files
EXTRA_DIST = extra

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

cdnf.1: $(top_srcdir)/configure.ac $(top_srcdir)/utils/cdnf-cmdline.ggo extra
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/utils/cdnf \
		--name="$(PACKAGE_NAME): canonic disjunctive normal form" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

configurator.1: $(top_srcdir)/configure.ac $(top_srcdir)/utils/configurator-cmdline.ggo extra
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/utils/configurator \
		--name="$(PACKAGE_NAME): configuration interface" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

MAINTAINERCLEANFILES = $(man_MANS) $(man_MANS:.1=.pdf)
EXTRA_DIST += $(man_MANS)

man-pdf: $(man_MANS:.1=.pdf)

%.pdf: %.1
	$(AM_V_GEN)groff -Tps -mandoc -c $< | ps2pdf - $(@:.1=.pdf)
