#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add your test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfile/' directory)
TESTFILES = bug1.fiona.og bug1.owfn \
            bug2.fiona.og bug2.owfn \
            bug3.fiona.og bug3.owfn \
            bug4.fiona.og bug4.owfn \
            bug5.fiona.og bug5.owfn \
            bug6.fiona.og bug6.owfn \
            bug7.fiona.og bug7.owfn \
            bug8.fiona.og bug8.owfn \
            bug9.fiona.og bug9.owfn \
            bug13651.owfn \
            bug13718.owfn \
            bug13994-1.owfn bug13994-2.owfn \
            bug14025.owfn \
            bug14150.owfn \
            carreservation_with_engine.owfn \
            error02.owfn \
            error03.owfn \
            error04-1.owfn error04-2.owfn \
            error06.owfn \
            error10.owfn \
            myCoffee.fiona.og myCoffee.owfn \
            phcontrol3.unf.fiona.og phcontrol3.unf.owfn \
            phil5.owfn \
            PO.fiona.og PO.owfn \
            QR.fiona.og QR.owfn \
	    regist-priv.owfn regist-priv.owfn.cov \
            seqTest1.fiona.og seqTest1.owfn \
            seqTest2.fiona.og seqTest2.owfn \
            seqTest3.fiona.og  seqTest3.owfn \
            sequence10.fiona.og sequence10.owfn \
            smtp3.fiona.og smtp3.owfn \
            T2.fiona.og T2.owfn \
            test-sync.owfn \
            zeus.fiona.og zeus.owfn \
	    LF_DF_controllable.owfn \
	    nLF_DF_controllable.owfn \
	    LF_DF_controllable2.owfn \
	    smartSendingEvent.owfn smartSendingEvent.fiona.og \
	    06-03-23_BPM06_shop_sect_6.owfn 06-03-23_BPM06_shop_sect_6.fiona.og


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at versions.sh wrap.sh \
             $(addprefix testfiles/,$(TESTFILES))

DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
