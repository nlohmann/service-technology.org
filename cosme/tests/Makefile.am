#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add your test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfiles/' directory)
TESTFILES = matching/AP-partner.owfn matching/AP-partner.sa matching/AP.og \
            matching/AP.owfn matching/BH-partner.owfn matching/BH-partner.sa \
            matching/BH.og matching/BH.owfn matching/CN-partner.owfn \
            matching/CN-partner.sa matching/CN.og matching/CN.owfn \
            matching/DG-partner.owfn matching/DG-partner.sa matching/DG.og \
            matching/DG.owfn matching/PO-partner.owfn matching/PO-partner.sa \
            matching/PO.og matching/PO.owfn matching/QR-partner.owfn \
            matching/QR-partner.sa matching/QR.og matching/QR.owfn \
            matching/RE-partner.owfn matching/RE-partner.sa matching/RE.og \
            matching/RE.owfn matching/RS-partner.owfn matching/RS-partner.sa \
            matching/RS.og matching/RS.owfn matching/SD-partner.owfn \
            matching/SD-partner.sa matching/SD.og matching/SD.owfn \
            matching/T2.owfn matching/TR-partner.owfn matching/TR-partner.sa \
            matching/TR.og matching/TR.owfn \
            matching/auctionService-partner.owfn \
            matching/auctionService-partner.sa matching/auctionService.og \
            matching/auctionService.owfn matching/interface-partner.owfn \
            matching/interface_match.og matching/internal.og \
            matching/internal_match.owfn \
            matching/loanApprovalProcess-partner.owfn \
            matching/loanApprovalProcess-partner.sa \
            matching/loanApprovalProcess.og matching/loanApprovalProcess.owfn \
            matching/myCoffee-partner.owfn matching/myCoffee-partner.sa \
            matching/myCoffee.og matching/myCoffee.owfn \
            matching/phcontrol3.unf-partner.owfn \
            matching/phcontrol3.unf-partner.sa matching/phcontrol3.unf.og \
            matching/phcontrol3.unf.owfn simulation/AP-pv.og simulation/AP.og \
            simulation/BH-pv.og simulation/BH.og simulation/CN-pv.og \
            simulation/CN.og simulation/DG-pv.og simulation/DG.og \
            simulation/PO-pv.og simulation/PO.og simulation/TR-pv.og \
            simulation/TR.og simulation/branch.og simulation/branch_chopped.og \
            simulation/branch_compact.og simulation/branch_compact_extend.og \
            simulation/branch_wrong_bits.og simulation/simple.og \
            simulation/simple_smaller_interface.og


# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sourses called 'cmdline.c'.
NO_COVER_FILES = */libs/* */*-lexic*.cc */*-syntax*.cc */cmdline.* */verbose.* */Output.*



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

################
# GNU autotest #
################

TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at wrap.sh \
             $(addprefix testfiles/,$(TESTFILES))
DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

# run the test suite when calling "make check"
check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

# clean the test results on "make clean"
clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

# call a specific test set, (e.g. "make check-basic")
check-%: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) --keywords=$*

# create the testsuite using autom4te
$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@


######################
# LCOV test coverage #
######################

# distribute the LGOV scripts
EXTRA_DIST += lcov/README lcov/gendesc lcov/genhtml lcov/geninfo lcov/genpng lcov/lcov zcov

# compile the code for code coverage
cover-prepare:
	$(MAKE) --directory=$(top_srcdir) clean all \
	  CXXFLAGS+="-fprofile-arcs -ftest-coverage" \
	  CFLAGS+="-fprofile-arcs -ftest-coverage" \
	  LDFLAGS+="-lgcov"

# get descriptions from the testsuite
descriptions: $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' -l > descriptions.txt
	$(SED) -i '1,5d;n;d;' descriptions.txt
	$(SED) -i 's/:[ \t]*testsuite.at:[0-9]*[ \t]*/ /;s/^[ \t]*//' descriptions.txt
	$(SED) -i 's/ /\n\t/' descriptions.txt
	lcov/gendesc descriptions.txt -o descriptions1
	cp descriptions1 descriptions10
	cp descriptions1 descriptions100
	$(SED) -i 's/^TN: \([1-9]\)$$/TN: 0\1/' descriptions10
	$(SED) -i 's/^TN: \([1-9]\)$$/TN: 00\1/' descriptions100
	$(SED) -i 's/^TN: \([1-9][1-9]\)$$/TN: 0\1/' descriptions100
	cat descriptions1 descriptions10 descriptions100 > descriptions
	rm descriptions1 descriptions10 descriptions100 descriptions.txt

# execute tests using coverage
cover-check: cover-prepare $(TESTSUITE)
	test ! -d cover || rm -fr cover
	$(MKDIR_P) cover
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) COVER=LCOV

# exclude generated code using the wildcards specified in NO_COVER_FILES
cover.info:
	cat cover/*.info > cover.info
	lcov/lcov --remove cover.info $(addprefix ",$(addsuffix ", $(NO_COVER_FILES))) -o cover.info

# create a HTML table with results from LCOV
cover-html: descriptions cover.info
	lcov/genhtml cover.info \
	   --output-directory cover-html --title "$(PACKAGE_STRING)" \
	   --show-details --legend --description-file descriptions --sort \
	   --no-function-coverage --prefix $(abs_top_builddir)
	@echo see `pwd`/cover-html/index.html for a coverage report

cover: cover-check cover-html

# use ZCOV for branch coverage
cover-branch: cover-prepare $(TESTSUITE)
	test ! -d cover || rm -fr cover
	$(MKDIR_P) cover
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) COVER=ZCOV
	zcov/zcov-merge cover/output.zcov cover/*.zcov
	zcov/zcov-genhtml cover/output.zcov cover-html
	@echo see `pwd`/cover-html/index.html for a coverage report

# clean also the temp files in the src directory
CLEANFILES = -fr cover descriptions 
MAINTAINERCLEANFILES = -fr cover-html cover.info
maintainer-clean-local:
	find $(top_builddir) -name *.gcov -exec rm {} \;
	find $(top_builddir) -name *.gcno -exec rm {} \;
	find $(top_builddir) -name *.gcda -exec rm {} \;
