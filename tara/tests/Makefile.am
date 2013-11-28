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

# <<-- CHANGE START (testfiles) -->>
TESTFILES = marvin2.lola marvin2.owfn marvin3.cf marvin3.owfn marvin.lola marvin.owfn myCoffeeCyclic_alt.owfn myCoffeeCyclic.owfn myCoffee.owfn phcontrol10.unf.owfn phcontrol3.unf.owfn phcontrol4.unf.owfn phcontrol5.unf.owfn phcontrol6.unf.owfn phcontrol6.unf.owfn.graph phcontrol6.unf.owfn.lola phcontrol7.unf.owfn phcontrol8.unf.owfn notControllable.owfn null.cf phCosts.cf simpleAlternative.owfn cyclic_simple.owfn cyclic_alternatives.owfn cyclic_conc.owfn cyclic_alternatives_strange.cf cyclic_alternatives_uc.owfn cyclic_conc_uc.owfn cyclic_conc.cf simpleReset.cf
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
NO_COVER_FILES = */libs/* */lexic_*.cc */syntax_*.cc */cmdline.* */verbose.* */Output.*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests

