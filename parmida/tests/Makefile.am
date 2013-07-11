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
# 2. Add the files you need for the test to the `testfiles' directory.
# 3. Add these files (without path) to the `TESTFILES' list.
# 4. Add the file to the SVN repository.

# <<-- CHANGE START (testfiles) -->>
TESTFILES = test1a.owfn test1b.owfn test1c.owfn test1d.owfn \
			test2a.owfn test2b.owfn test2c.owfn test2d.owfn \
			test3a.owfn test3b.owfn \
			test4a.owfn test4b.owfn test4c.owfn test4d.owfn test4e.owfn test4f.owfn \
			test5a.owfn test5b.owfn test5c.owfn test5d.owfn test5e.owfn test5f.owfn \
			test6a.owfn test6b.owfn test6c.owfn test6d.owfn test6e.owfn test6f.owfn \
			test7a.owfn test7b.owfn test7c.owfn test7d.owfn test7e.owfn test7f.owfn \
			test8a.owfn test8b.owfn test8c.owfn test8d.owfn test8e.owfn test8f.owfn test8g.owfn \
			test9a.owfn test9b.owfn test9c.owfn test9d.owfn \
			test10a.owfn test11a.owfn test11b.owfn \
			test12a.owfn test12b.owfn test12c.owfn test12d.owfn test12e.owfn \ 
			test12f.owfn test12g.owfn test12h.owfn test12i.owfn \ 
			test13a.owfn

# <<-- CHANGE END -->>



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
NO_COVER_FILES = */libs/* */lexic_*.cc */syntax_*.cc */cmdline.* */verbose.* */Output.* */JSON.* */Runtime.* */Socket.*


#------------------------------#
# Part 2a: Tool-specific fixes #
#------------------------------#

# If LoLA is not installed, the path to the binary needs to be passed as an
# environment variable to the test scripts.
#TESTSUITEFLAGS = LOLA=$(LOLA)

#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests

