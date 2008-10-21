man_MANS = $(PACKAGE).1

MAINTAINERCLEANFILES = $(man_MANS)

DISTCLEANFILES = $(man_MANS) Makefile.in

EXTRA_DIST = $(man_MANS)

$(PACKAGE).1: $(top_srcdir)/configure.ac
	$(HELP2MAN) --name="Functional InteractiON Analysis of open workflow nets" ../src/$(PACKAGE) --no-info --output=$@  --manual="service-technology.org" --source="service-technology.org" --help-option="--help"
