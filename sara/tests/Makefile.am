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
            test4.owfn test4.sara test5.owfn test5.sara test5b.sara test6.owfn \
            test6.sara test7.sara test7_1.owfn test7_2.owfn test7_3.owfn \
            test7_4.owfn test7_5.owfn test7_6.owfn test7_7.owfn \
            test7_8.owfn test7_9.owfn test7_X.owfn test8.sara test8_1.owfn \
            test8_1.sara test8_2.owfn test8_2.sara test8_3.owfn test8_3.sara \
            test8_4.owfn test8_4.sara test8_5.owfn test8_5.sara test8_6.owfn \
            test8_6.sara test8_7.owfn test8_7.sara test8_8.owfn test8_8.sara \
            test8_9.owfn test8_9.sara test8_X.owfn test8_X.sara test9.owfn \
            test9.sara tests.pdf garavel.llnet.sara garavel.llnet test16.sara test16.owfn \
			MAPK320.pnml MAPK320.sara ce.owfn ce.sara ce2.owfn ce2.sara \
			ce3.owfn ce3.sara ce4.owfn ce4.sara ce5.owfn ce5.sara


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

include $(top_srcdir)/m4/Makefile.tests
