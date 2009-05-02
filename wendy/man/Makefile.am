man_MANS = $(PACKAGE).1

MAINTAINERCLEANFILES = $(man_MANS)

EXTRA_DIST = $(man_MANS)

$(PACKAGE).1: $(top_srcdir)/configure.ac
	$(HELP2MAN) --name="Calculating Operating Guidelines" ../src/$(PACKAGE) --no-info --output=$@  --manual="service-technology.org" --source="service-technology.org" --help-option="--full-help"
