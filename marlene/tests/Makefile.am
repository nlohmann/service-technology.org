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
TESTFILES = test.owfn test.ar \
	    test-n.owfn test-n.ar \
	    ca1.owfn ca1_sync.owfn ca2.owfn td.owfn td_cyclic.owfn \
	    td.ar td_sync.ar td_sync2.ar \
		td_ret.owfn td_ret.ar \
	    nopetrify.conf \
	    wrongsyntax.owfn wrongsyntax.ar wronglexic.ar \
	    sync1_exp.owfn sync2_exp.owfn sync3_exp.owfn \
	    async1_exp.owfn async2_exp.owfn async3_exp.owfn \
	    bug_compplaces.owfn bug_compplaces.ar \
      some-rules.ar \
      d1.owfn d2.owfn diag0.ar diag1.ar diag2.ar


#-----------------------------------------#
# Part 2: Exclusions for the Cover Script #
#-----------------------------------------#

# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sources called 'cmdline.c'.
NO_COVER_FILES = */libs/* */parser_adapt_rules.cc */lexer_adapt_rules.cc */cmdline.* */Output.* \
		 */lexer_marking_information.cc */parser_marking_information.cc



#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests

