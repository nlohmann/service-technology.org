TESTSUITE = $(builddir)/testsuite

EXTRA_DIST = testsuite.at \
             testfiles/myCoffee.wendy.og \
             testfiles/myCoffee.fiona.og \
             testfiles/phcontrol3.unf.wendy.og \
             testfiles/phcontrol3.unf.fiona.og \
             testfiles/PO.wendy.og \
             testfiles/PO.fiona.og \
             testfiles/sequence10.wendy.og \
             testfiles/sequence10.fiona.og \
             testfiles/smtp3.wendy.og \
             testfiles/smtp3.fiona.og \
             testfiles/zeus.wendy.og \
             testfiles/zeus.fiona.og

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

