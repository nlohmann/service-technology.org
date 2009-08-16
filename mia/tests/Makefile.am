#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add you test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfile/' directory)
TESTFILES = reg_old.owfn reg_new.owfn \
            sequence5.owfn sequence5-pv.owfn \
            ex1.owfn ex2.owfn \
            PO.owfn PO-pv.owfn


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
