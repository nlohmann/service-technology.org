#============================================================================#
# AUTOTEST MAKEFILE                                                          #
# skeleton written by Niels Lohmann <niels.lohmann@uni-rostock.de>           #
#============================================================================#

# This Makefile organizes the generation of the test suite, the code coverage
# and the packaging of the tests. It consists of three parts, of which you
# need to change the first if you want to add test files and the second if
# you want to exclude files from the test case coverage (very seldomly). The
# last part is generic and you must not change it.


#--------------------#
# Part 1: Test Files #
#--------------------#

# If you want to use a new test file in a test case, proceed as follows:
# 1. Edit the file `testsuite.at' and add your test case
# 2. Add the files you need for the test to the `testfile' directory.
# 3. Add these files (without path) to the `TESTFILES' list.
# 4. Add the file to the SVN repository.
TESTFILES = test1.owfn test1.sara test10.owfn test10.sara test11.owfn \
            test11.sara test12.sara test13.sara test14.sara test15.owfn \
            test15.sara test2.owfn test2.sara test3.owfn test3.sara \
            test4.owfn test4.sara test5.owfn test5.sara test6.owfn \
            test6.sara test7.sara test7_1.owfn test7_2.owfn test7_3.owfn \
            test7_4.owfn test7_5.owfn test7_6.owfn test7_7.owfn \
            test7_8.owfn test7_9.owfn test7_X.owfn test8.sara test8_1.owfn \
            test8_1.sara test8_2.owfn test8_2.sara test8_3.owfn test8_3.sara \
            test8_4.owfn test8_4.sara test8_5.owfn test8_5.sara test8_6.owfn \
            test8_6.sara test8_7.owfn test8_7.sara test8_8.owfn test8_8.sara \
            test8_9.owfn test8_9.sara test8_X.owfn test8_X.sara test9.owfn \
            test9.sara tests.pdf garavel.llnet.sara garavel.llnet


#-----------------------------------------#
# Part 2: Exclusions for the Cover Script #
#-----------------------------------------#

# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sourses called 'cmdline.c'.
NO_COVER_FILES = */libs/* */lexic_*.cc */syntax_*.cc */cmdline.* */verbose.* */Output.* */Results.*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

#--------------#
# GNU autotest #
#--------------#

TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at wrap.sh $(addprefix testfiles/,$(TESTFILES))
DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

# run the test suite when calling "make check"
check-local: atconfig $(TESTSUITE)
	$(AM_V_at)$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

# clean the test results on "make clean"
clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

# call a specific test set by keyword (e.g. "make check-basic")
check-%: atconfig $(TESTSUITE)
	$(AM_V_at)$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) --keywords=$*

# create the testsuite using autom4te
$(TESTSUITE): $(srcdir)/testsuite.at package.m4
	$(AM_V_GEN)$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	$(AM_V_at)mv $@.tmp $@


#---------------#
# test coverage #
#---------------#

# distribute the LGOV scripts
EXTRA_DIST += lcov/README lcov/gendesc lcov/genhtml lcov/geninfo lcov/genpng \
              lcov/lcov zcov zcov/GCovGroup.py zcov/GCovParser.py \
              zcov/style.css zcov/zcov-genhtml zcov/zcov-merge zcov/zcov-scan \
              zcov/zcov-summarize zcov/js/sorttable.js zcov/js/sourceview.js

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
