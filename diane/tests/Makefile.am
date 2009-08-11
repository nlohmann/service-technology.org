TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at versions.sh wrap.sh \
		testfiles/PO.owfn.lola \
		testfiles/T2.owfn.lola \
		testfiles/choreography.lola \
		testfiles/phils.lola \
		testfiles/zeus.lola

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
