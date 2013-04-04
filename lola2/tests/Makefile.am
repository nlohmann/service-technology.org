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
TESTFILES = Kanban5.pnml.lola Kanban5.5safe.pnml.lola 4state.lola \
            eratosthenes-020.pnml.lola garavel.lola garavel.task process.lola \
            phils5.lola phils10.lola phils1000.lola phils10000.lola \
            phils5dld.lola phils10dld.lola \
            syntax_min.lola syntax_full.lola syntax_utf8.lola \
            test_significant.lola philsinitial.task philsunsat.task \
	    	m.lola philpos.task planner.lola LTL/cycle.lola LTL/cycle2.lola LTL/cycle3.lola\
	    	LTL/cycle4.lola LTL/cycle5.lola LTL/cycle6.lola LTL/simple.buechi LTL/cycleprop.buechi\
	    	LTL/cycle_weak1.lola LTL/cycle_weak2.lola LTL/cycle_weak3.lola LTL/cycle_weak4.lola \
	    	LTL/phils5dld.buechi LTL/fork1.buechi \
	    	CTL/phils-AX.task CTL/phils-EX.task CTL/phils-TemporalConjunction.task\
	    	CTL/phils-TemporalDisjunction.task CTL/phils-complex.task\
		echo-d3r3.name.lola
	    


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
NO_COVER_FILES = */ParserPTNetLoLA.cc */Lexic.cc */cmdline.* */libs/*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
