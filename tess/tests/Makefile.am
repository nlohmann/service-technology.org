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
TESTFILES = EMPTY myCoffee.og \
            myCoffee_insert_ref.dot myCoffee_insert_ref.eaa \
            myCoffee_delete_ref.dot myCoffee_delete_ref.eaa \
            myCoffee_rename_ref.dot myCoffee_rename_ref.eaa \
            myCoffee_all_ref.dot myCoffee_all_ref.eaa \
            small.og small_TestOG_ref.dot small_TestOG_ref.eaa small_TestCases_ref.dot small_TestCases_ref.eaa   \
            final.og final_TestOG_ref.dot final_TestOG_ref.eaa final_TestCases_ref.dot final_TestCases_ref.eaa  \
            true.og  true_TestOG_ref.dot  true_TestOG_ref.eaa  true_TestCases_ref.dot  true_TestCases_ref.eaa 


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
NO_COVER_FILES = */minisat/* */test* */og_lexer.cc */og_syntax.cc */cmdline.* */verbose.* */Output.*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
