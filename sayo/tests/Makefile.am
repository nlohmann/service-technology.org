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
TESTFILES = NonFinalTauNode1.fiona.og \
            NonFinalTauNode1.wendy.og \
            NonFinalTauNode1not.fiona.og \
            NonFinalTauNode1not.wendy.og \
            NonFinalTauNode2.fiona.og \
            NonFinalTauNode2.wendy.og \
            NonFinalTauNode2not.fiona.og \
            NonFinalTauNode2not.wendy.og \
            NonFinalTauNode3.fiona.og \
            NonFinalTauNode3.wendy.og \
            NonFinalTauNode4.fiona.og \
            NonFinalTauNode4.wendy.og \
            PO.fiona.og \
            PO.wendy.og \
            complexPV.fiona.og \
            complexPV.wendy.og \
            error1.og \
            error2.og \
            error3.og \
            myCoffee.fiona.og \
            myCoffee.wendy.og \
            non_empty_true_node2.fiona.og \
            non_empty_true_node2.wendy.og \
            non_empty_true_node3.fiona.og \
            non_empty_true_node3.wendy.og \
            phcontrol3.unf.fiona.og \
            phcontrol3.unf.wendy.og \
            zeus.fiona.og \
            zeus.wendy.og


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
NO_COVER_FILES = */cmdline.c */lexic_og.cc */syntax_og.cc


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests

