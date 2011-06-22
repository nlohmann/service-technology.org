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
TESTFILES = bug15282.lola \
            bug12922.lola \
            bug13988.lola bug13988-1.task bug13988-2.task \
            bug17421.lola bug17421.statepredicate bug17421.ctlformula \
            choreography.lola \
            choreography.tar.gz \
            data.lola \
            echo.lola \
            echo.task \
            garavel.lola \
            phils.lola \
            planner.lola \
            planner.task \
            process.lola \
            process.task \
            umlprocess.lola \
            unbounded.lola \
            FMS10.pnml.lola \
            fNOKFMSM01.task \
            fNOKFMSM02.task \
            fNOKFMSM03.task \
            fNOKFMSM04.task \
            fNOKFMSM05.task \
            fNOKFMSM06.task \
            fNOKFMSM07.task \
            fNOKFMSM08.task \
            fNOKFMSM09.task \
            fNOKFMSM10.task \
            fOKFMSM01.task \
            fOKFMSM02.task \
            fOKFMSM03.task \
            fOKFMSM04.task \
            fOKFMSM05.task \
            fOKFMSM06.task \
            fOKFMSM07.task \
            fOKFMSM08.task \
            fOKFMSM09.task \
            fOKFMSM10.task \
            MAPK8.pnml.lola \
            fNOKMAPK01.task \
            fNOKMAPK02.task \
            fNOKMAPK03.task \
            fNOKMAPK04.task \
            fNOKMAPK05.task \
            fNOKMAPK06.task \
            fNOKMAPK07.task \
            fNOKMAPK08.task \
            fNOKMAPK09.task \
            fNOKMAPK10.task \
            fOKMAPK01.task \
            fOKMAPK02.task \
            fOKMAPK03.task \
            fOKMAPK04.task \
            fOKMAPK05.task \
            fOKMAPK06.task \
            fOKMAPK07.task \
            fOKMAPK08.task \
            fOKMAPK09.task \
            fOKMAPK10.task \
            Kanban5.pnml.lola \
            fNOKKBNM01.task \
            fNOKKBNM02.task \
            fNOKKBNM03.task \
            fNOKKBNM04.task \
            fNOKKBNM05.task \
            fNOKKBNM06.task \
            fNOKKBNM07.task \
            fNOKKBNM08.task \
            fNOKKBNM09.task \
            fNOKKBNM10.task \
            fOKKBNM01.task \
            fOKKBNM02.task \
            fOKKBNM03.task \
            fOKKBNM04.task \
            fOKKBNM05.task \
            fOKKBNM06.task \
            fOKKBNM07.task \
            fOKKBNM08.task \
            fOKKBNM09.task \
            fOKKBNM10.task


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
NO_COVER_FILES = */readnet-lexic.cc */readnet-syntax.cc */cmdline.*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
