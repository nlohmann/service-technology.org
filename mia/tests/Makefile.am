TESTSUITE = $(builddir)/testsuite

EXTRA_DIST = testsuite.at \
             testfiles/reg_old.owfn testfiles/reg_new.owfn \
             testfiles/sequence5.owfn testfiles/sequence5-pv.owfn \
             testfiles/ex1.owfn testfiles/ex2.owfn

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

