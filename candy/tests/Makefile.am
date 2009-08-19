TESTSUITE = $(builddir)/testsuite

EXTRA_DIST = testsuite.at wrap.sh \
             testfiles/costOG1.og \
             testfiles/costOG1.cf \
             testfiles/costOG1.owfn \
             testfiles/costOG1_expected.og \
             testfiles/costOG2.og \
             testfiles/costOG2.og2 \
             testfiles/costOG2.cf \
             testfiles/costOG2.owfn \
             testfiles/costOG2_expected.og \
             testfiles/error_incomplete_interface.og \
             testfiles/error_overfull_interface.og \
             testfiles/error_incomplete_nodes.og \
             testfiles/error_overfull_nodes.og \
             testfiles/error_incomplete.cf \
             testfiles/error_overfull.cf \
             testfiles/error_repetitions.cf \
             testfiles/error_incomplete.owfn \
             testfiles/error_overfull.owfn \
             testfiles/error_weights.owfn \
             testfiles/error_shared.owfn \
             testfiles/error_nonnormal.owfn

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

