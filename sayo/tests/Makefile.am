TESTSUITE = $(builddir)/testsuite

EXTRA_DIST = testsuite.at \
             testfiles/NonFinalTauNode1.fiona.og \
             testfiles/NonFinalTauNode1.wendy.og \
             testfiles/NonFinalTauNode1not.fiona.og \
             testfiles/NonFinalTauNode1not.wendy.og \
             testfiles/NonFinalTauNode2.fiona.og \
             testfiles/NonFinalTauNode2.wendy.og \
             testfiles/NonFinalTauNode2not.fiona.og \
             testfiles/NonFinalTauNode2not.wendy.og \
             testfiles/NonFinalTauNode3.fiona.og \
             testfiles/NonFinalTauNode3.wendy.og \
             testfiles/NonFinalTauNode4.fiona.og \
             testfiles/NonFinalTauNode4.wendy.og \
             testfiles/PO.fiona.og \
             testfiles/PO.wendy.og \
             testfiles/complexPV.fiona.og \
             testfiles/complexPV.wendy.og \
             testfiles/error1.og \
             testfiles/error2.og \
             testfiles/myCoffee.fiona.og \
             testfiles/myCoffee.wendy.og \
             testfiles/non_empty_true_node2.fiona.og \
             testfiles/non_empty_true_node2.wendy.og \
             testfiles/non_empty_true_node3.fiona.og \
             testfiles/non_empty_true_node3.wendy.og \
             testfiles/phcontrol3.unf.fiona.og \
             testfiles/phcontrol3.unf.wendy.og \
             testfiles/zeus.fiona.og \
             testfiles/zeus.wendy.og

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

