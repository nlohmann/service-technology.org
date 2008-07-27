man_MANS = owfn2bpel.1
MAINTAINERCLEANFILES = $(man_MANS)

EXTRA_DIST = $(man_MANS)

owfn2bpel.1: $(top_srcdir)/configure.ac
	$(HELP2MAN) ../src/owfn2bpel --no-info --output=$@ --name="translates a Petri net model into an abstract BPEL process" --manual="service-technology.org" --source="service-technology.org" --help-option="--full-help"
