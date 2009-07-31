TESTSUITE = $(builddir)/testsuite

EXTRA_DIST = testsuite.at wrap.sh \
             testfiles/costOG1.og \
             testfiles/costOG1.cf \
             testfiles/costOG2.og \
             testfiles/costOG2.cf

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

