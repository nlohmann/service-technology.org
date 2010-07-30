#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add your test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfiles/' directory)
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

include $(top_srcdir)/m4/Makefile.tests
