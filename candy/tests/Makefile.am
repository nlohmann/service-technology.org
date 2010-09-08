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
TESTFILES = costOG1.og \
            costOG1.cf \
            costOG1.owfn \
            costOG1.expected.og \
            costOG2.og \
            costOG2.og2 \
            costOG2.cf \
            costOG2.owfn \
            costOG2.expected.og \
            error_incomplete_interface.og \
            error_overfull_interface.og \
            error_incomplete_nodes.og \
            error_overfull_nodes.og \
            error_incomplete.cf \
            error_overfull.cf \
            error_repetitions.cf \
            error_incomplete.owfn \
            error_overfull.owfn \
            error_weights.owfn \
            error_shared.owfn \
            error_nonnormal.owfn \
            formula_flatten_and.og \
            formula_flatten_and.expected.og \
            formula_flatten_or.og \
            formula_flatten_or.expected.og \
            formula_flatten_andor.og \
            formula_flatten_andor.expected.og \
            formula_flatten_sizezero.og \
            formula_flatten_sizezero.expected.og \
            formula_flatten_sizeone.og \
            formula_flatten_sizeone.expected.og \
            formula_flatten_sizetwo.og \
            formula_flatten_not.og \
            formula_flatten_not.expected.og \
            formula_flatten.cf \
            formula_merge_and.og \
            formula_merge_and.expected.og \
            formula_merge_or.og \
            formula_merge_or.expected.og \
            formula_merge_andor.og \
            formula_merge_andor.expected.og \
            formula_merge.cf \
            formula_minimal_and.og \
            formula_minimal_and.expected.og \
            formula_minimal_or.og \
            formula_minimal_andor.og \
            formula_minimal_not.og \
            formula_minimal_not.expected.og \
            formula_minimal_true.og \
            formula_minimal_false.og \
            formula_minimal_false.expected.og



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


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
