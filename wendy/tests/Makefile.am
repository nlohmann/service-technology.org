#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add your test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfiles/' directory)
TESTFILES = 06-03-23_BPM06_shop_sect_6.fiona.og \
            06-03-23_BPM06_shop_sect_6.owfn AP.owfn LF_DF_controllable.owfn \
            LF_DF_controllable2.owfn Lohmann_2008_wsfm-Fig2b.owfn \
            PO.fiona.og PO.owfn QR.fiona.og QR.owfn SaScSdRb.owfn \
            T2.fiona.og T2.owfn bits.owfn bug1.fiona.og bug1.owfn bug10.owfn \
            bug11.owfn bug13651.owfn bug13718.owfn bug13994-1.owfn \
            bug13994-2.owfn bug14025.owfn bug14150.owfn bug14158.owfn \
            bug14166.owfn bug2.fiona.og bug2.owfn bug3.fiona.og bug3.owfn \
            bug4.fiona.og bug4.owfn bug5.fiona.og bug5.owfn bug6.fiona.og \
            bug6.owfn bug7.fiona.og bug7.owfn bug8.fiona.og bug8.owfn \
            bug9.fiona.og bug9.owfn carreservation_with_engine.owfn \
            coverFiona_Nc1.owfn coverFiona_Nc1.owfn.cov coverFiona_Nc2.owfn \
            coverFiona_Nc2.owfn.cov coverFiona_Nc3.owfn \
            coverFiona_Nc3.owfn.cov dead.owfn error02.owfn error03.owfn \
            error04-1.owfn error04-2.owfn error06.owfn error10.owfn \
            error16.1.cov error16.2.cov genericNonTrivialSCC.owfn \
            genericSynchLFQAP.owfn marlene-ca.owfn marlene-td.ar \
            marlene-td.owfn mb.owfn mia-PO-pv.owfn mia-PO.owfn \
            mia-reg_new.owfn mia-reg_old.owfn myCoffee.fiona.og \
            myCoffee.owfn nLF_DF_controllable.owfn nlc.owfn \
            phcontrol3.unf.fiona.og phcontrol3.unf.owfn phcontrol6.unf.owfn \
            phcontrol9.unf.owfn phil5.owfn regist-priv.owfn \
            regist-priv.owfn.cov seqTest1.fiona.og seqTest1.owfn \
            seqTest2.fiona.og seqTest2.owfn seqTest3.fiona.og seqTest3.owfn \
            sequence10.fiona.og sequence10.owfn small.owfn \
            smartSendingEvent.fiona.og smartSendingEvent.owfn \
            smtp3.fiona.og smtp3.owfn sync_cover.owfn sync_cover.owfn.cov \
            sync_deadlock.owfn test-sync.owfn uncontrollable.owfn \
            zeus.fiona.og zeus.owfn



# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sourses called 'cmdline.c'.
NO_COVER_FILES = */libs/* */lexic_*.cc */syntax_*.cc */cmdline.* */verbose.* */Output.*



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


##############
# BENCHMARKS #
##############

EXTRA_DIST += benchmarks.at

# create the benchmark script using autom4te
benchmarks: $(srcdir)/benchmarks.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

bench: atconfig benchmarks
	test ! -d benchmarks-results || rm -fr benchmarks-results
	$(MKDIR_P) benchmarks-results
	$(SHELL) 'benchmarks' $(TESTSUITEFLAGS)
