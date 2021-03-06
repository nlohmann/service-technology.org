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
TESTFILES = myCoffee.owfn test2_MAX.owfn test2_MAX_MAX.owfn testunbounded.owfn \
		test2.owfn test2err.owfn C_MAX.owfn \
		test2_BSD.dot test2_CSD.dot test2_MAX_BSD.dot test2_MAX_MP_CSD.dot \
		error02.owfn error03.owfn error04-1.owfn error04-2.owfn \
		error06.owfn error08.owfn \
		error10-1-1.dot error10-1-2.dot error10-1-3.dot error10-1-4.dot \
		error10-2-1.dot error10-2-2.dot error10-3.dot \
		error10-4-1.dot error10-4-2.dot \
		test2_CSD_neg-1.dot test2_CSD_neg-2.dot test2_CSD_neg-3.dot \
		test2_CSD_neg-4.dot test2_CSD_neg-5.dot \
		test2_MAX_BSD_neg-1.dot test2_MAX_BSD_neg-2.dot \
		test2_MAX_BSD_neg-3.dot test2_MAX_BSD_neg-4.dot


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
NO_COVER_FILES = */libs/* */lexic_*.cc */syntax_*.cc */cmdline.* */verbose.* */Output.* */Results.*


#------------------------------#
# Part 2a: Tool-specific fixes #
#------------------------------#

# If LoLA is not installed, the path to the binary needs to be passed as an
# environment variable to the test scripts.
TESTSUITEFLAGS = LOLA=$(LOLA)


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
