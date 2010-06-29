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
TESTFILES = 06-03-23_BPM06_shop_sect_6.fiona.og \
            06-03-23_BPM06_shop_sect_6.owfn AP.owfn LF_DF_controllable_1.owfn \
            LF_DF_controllable_2.owfn Lohmann_2008_wsfm-Fig2b.owfn \
            PO.fiona.og PO.owfn QR.fiona.og QR.owfn SaScSdRb.owfn \
            T2.fiona.og T2.owfn bits.owfn bug1.fiona.og bug1.owfn bug10.owfn \
            bug11.owfn bug13651.owfn bug13718.owfn bug13994-1.owfn \
            bug13994-2.owfn bug14025.owfn bug14150.owfn bug14158.owfn \
            bug14166.owfn bug2.fiona.og bug2.owfn bug3.fiona.og bug3.owfn \
            bug4.fiona.og bug4.owfn bug5.fiona.og bug5.owfn bug6.fiona.og \
            bug6.owfn bug7.fiona.og bug7.owfn bug8.fiona.og bug8.owfn \
            bug9.fiona.og bug9.owfn carreservation_with_engine.owfn \
            coverFiona_Nc1.owfn coverFiona_Nc1.owfn.cov coverFiona_Nc2.owfn \
            coverFiona_Nc2.owfn.cov coverFiona_Nc3.owfn \
            coverFiona_Nc3.owfn.cov dead.owfn error02.owfn error03.owfn \
            error04-1.owfn error04-2.owfn error06.owfn error10.owfn \
            error16.1.cov error16.2.cov genericNonTrivialSCC.owfn \
            genericSynchLFQAP.owfn marlene-ca.owfn marlene-td.ar \
            marlene-td.owfn mb.owfn mia-PO-pv.owfn mia-PO.owfn \
            mia-reg_new.owfn mia-reg_old.owfn myCoffee.fiona.og \
            myCoffee.owfn nLF_DF_controllable.owfn nlc.owfn \
            LF_DF_controllable_3.owfn LF_DF_controllable_4.owfn \ 
            phcontrol3.unf.fiona.og phcontrol3.unf.owfn phcontrol6.unf.owfn \
            phcontrol9.unf.owfn phil5.owfn regist-priv.owfn \
            regist-priv.owfn.cov seqTest1.fiona.og seqTest1.owfn \
            seqTest2.fiona.og seqTest2.owfn seqTest3.fiona.og seqTest3.owfn \
            sequence10.fiona.og sequence10.owfn small.owfn \
            smartSendingEvent.fiona.og smartSendingEvent.owfn \
            smtp3.fiona.og smtp3.owfn sync_cover.owfn sync_cover.owfn.cov \
            sync_deadlock.owfn test-sync.owfn uncontrollable.owfn \
            zeus.fiona.og zeus.owfn bignode1500000.owfn \
            bug14693.owfn bug14719.owfn \
            CB.owfn CR.owfn TPO.owfn bug14782.owfn bug14798.owfn \
            phils_3_1.owfn RS.owfn cyclicFullCoffee.pnml \
            moreThanOneFinalLiteral.owfn innerCyclicMPPacyclic.owfn


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
