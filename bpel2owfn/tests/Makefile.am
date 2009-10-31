#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add you test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfile/' directory)
TESTFILES = OrderingServiceProcess.bpel auction.bpel auction.wsdl auctionService.bpel auctionServiceInterface.wsdl chor_paper_agency.bpel chor_paper_airline.bpel chor_paper_topology1.xml chor_paper_topology10.xml chor_paper_traveler.bpel coffee.bpel customer1.bpel customer2.bpel customer3.bpel error.bpel loan.bpel loan.wsdl loanApprovalProcess.bpel loanServicePT.wsdl orderingLT.wsdl orderingPT.wsdl orderingProperties.wsdl purchase.bpel purchase.wsdl purchaseOrderProcess.bpel purchaseOrderProcess.dot purchaseOrderProcess.wsdl sa00002.bpel sa00002.wsdl sa00003.bpel sa00005.bpel sa00005.wsdl sa00006.bpel sa00007.bpel sa00008.bpel sa00015.bpel sa00016.bpel sa00017.bpel sa00018.bpel sa00023.bpel sa00024.bpel sa00025.bpel sa00032.bpel sa00035.bpel sa00036.bpel sa00037.bpel sa00044.bpel sa00048.bpel sa00048.wsdl sa00051.bpel sa00052.bpel sa00055.bpel sa00056.bpel sa00057.bpel sa00058.bpel sa00058.wsdl sa00059.bpel sa00060.bpel sa00061.bpel sa00062.bpel sa00063.bpel sa00064.bpel sa00065.bpel sa00066.bpel sa00067.bpel sa00068.bpel sa00069.bpel sa00070.bpel sa00071.bpel sa00072.bpel sa00073.bpel sa00074.bpel sa00075.bpel sa00076.bpel sa00077.bpel sa00078.bpel sa00079.bpel sa00080.bpel sa00081.bpel sa00082.bpel sa00083.bpel sa00084.bpel sa00087.bpel sa00087.wsdl sa00088.bpel sa00091.bpel sa00092.bpel sa00093.bpel shipping.bpel shippingLT.wsdl shippingPT.wsdl shippingProperties.wsdl shippingService.bpel


# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sourses called 'cmdline.c'.
NO_COVER_FILES = */libs/* */cmdline.c */verbose.* \
                 */ast-config.cc */ast-grammar.cc */ast-system-k.cc \
                 */ast-system-k.h */ast-system-rk.cc */ast-system-rk.h \
                 */ast-system-unpk.cc */ast-system-unpk.h \
                 */frontend-lexer.cc \
                 */frontend-parser-chor.cc */frontend-parser-wsdl.cc \
                 */frontend-parser.cc */petrinet-patterns-basic.cc \
                 */petrinet-patterns-handlers.cc \
                 */petrinet-patterns-links.cc \
                 */petrinet-patterns-scopes.cc \
                 */petrinet-patterns-shortcuts.cc \
                 */petrinet-patterns-structured.cc


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
	$(AM_V_at)$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

# clean the test results on "make clean"
clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

# call a specific test set, (e.g. "make check-basic")
check-%: atconfig $(TESTSUITE)
	$(AM_V_at)$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) --keywords=$*

# create the testsuite using autom4te
$(TESTSUITE): $(srcdir)/testsuite.at package.m4
	$(AM_V_GEN)$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	$(AM_V_at)mv $@.tmp $@


######################
# LCOV test coverage #
######################

# distribute the LGOV scripts
EXTRA_DIST += lcov/README lcov/gendesc lcov/genhtml lcov/geninfo lcov/genpng lcov/lcov

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
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) COVER=TRUE

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

# clean also the temp files in the src directory
CLEANFILES = -fr cover descriptions 
MAINTAINERCLEANFILES = -fr cover-html cover.info
maintainer-clean-local:
	find $(top_builddir) -name *.gcov -exec rm {} \;
	find $(top_builddir) -name *.gcno -exec rm {} \;
	find $(top_builddir) -name *.gcda -exec rm {} \;
