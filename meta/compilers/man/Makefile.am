# the manpages to create
man_MANS = fiona2wendy.1 ig2sa.1 sa2ig.1 wendy2fiona.1 wendyExpandBits.1 wendyFormula2bits.1 wendyReduceBits.1

# additional files
EXTRA_DIST = 

fiona2wendy.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/fiona2wendy \
		--name="$(PACKAGE_NAME) fiona2wendy" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

ig2sa.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/ig2sa \
		--name="$(PACKAGE_NAME) ig2sa" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

sa2ig.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/sa2ig \
		--name="$(PACKAGE_NAME) sa2ig" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

wendy2fiona.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/wendy2fiona \
		--name="$(PACKAGE_NAME) wendy2fiona" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

wendyExpandBits.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/wendyExpandBits \
		--name="$(PACKAGE_NAME) wendyExpandBits" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

wendyFormula2bits.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/wendyFormula2bits \
		--name="$(PACKAGE_NAME) wendyFormula2bits" \
		--output=$@ \
		--manual="service-technology.org" \
		--source="service-technology.org" \
		--help-option="--help" \
		--no-info

wendyReduceBits.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/cmdline.ggo
	$(AM_V_GEN)$(HELP2MAN) \
		$(top_builddir)/src/wendyReduceBits \
		--name="$(PACKAGE_NAME) wendyReduceBits" \
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
