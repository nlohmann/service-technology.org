TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at versions.sh wrap.sh \
		testfiles/myCoffee.fiona.og testfiles/myCoffee.owfn \
		testfiles/PO.fiona.og testfiles/PO.owfn \
		testfiles/T2.fiona.og testfiles/T2.owfn \
		testfiles/bug1.fiona.og testfiles/bug1.owfn \
		testfiles/bug2.fiona.og testfiles/bug2.owfn \
		testfiles/bug3.fiona.og testfiles/bug3.owfn \
		testfiles/bug4.fiona.og testfiles/bug4.owfn \
		testfiles/bug5.fiona.og testfiles/bug5.owfn \
		testfiles/bug6.fiona.og testfiles/bug6.owfn \
		testfiles/bug7.fiona.og testfiles/bug7.owfn \
		testfiles/bug8.fiona.og testfiles/bug8.owfn \
		testfiles/bug9.fiona.og testfiles/bug9.owfn \
		testfiles/bug13651.owfn \
		testfiles/bug13718.owfn \
		testfiles/error02.owfn \
		testfiles/error03.owfn \
		testfiles/error04-1.owfn testfiles/error04-2.owfn \
		testfiles/error06.owfn \
		testfiles/error10.owfn \
		testfiles/phcontrol3.unf.fiona.og testfiles/phcontrol3.unf.owfn \
		testfiles/sequence10.fiona.og testfiles/sequence10.owfn \
		testfiles/smtp3.fiona.og testfiles/smtp3.owfn \
		testfiles/zeus.fiona.og testfiles/zeus.owfn \
		testfiles/phil5.owfn

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
