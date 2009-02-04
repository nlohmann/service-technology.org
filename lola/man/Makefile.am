man_MANS = $(PACKAGE).1

MAINTAINERCLEANFILES = $(man_MANS)

EXTRA_DIST = $(man_MANS)

$(PACKAGE).1: $(top_srcdir)/configure.ac
	$(HELP2MAN) --name="LoLA (a Low Level Petri Net Analyzer)" ../src/$(PACKAGE) --no-info --output=$@  --manual="service-technology.org" --source="service-technology.org" --help-option="--full-help"
