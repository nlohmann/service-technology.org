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
TESTFILES = OGs/AP.bits.og \
            OGs/AP.og \
            OGs/AP-pv.og \
            OGs/auctionService.bits.og \
            OGs/auctionService.og \
            OGs/BH.bits.og \
            OGs/BH.og \
            OGs/BH-pv.og \
            OGs/branch_chopped.og \
            OGs/branch_compact_extend.og \
            OGs/branch_compact.og \
            OGs/branch.og \
            OGs/branch_wrong_bits.og \
            OGs/CN.bits.og \
            OGs/CN-neu.og \
            OGs/CN.og \
            OGs/CN-pv.og \
            OGs/DG.bits.og \
            OGs/DG.og \
            OGs/DG-pv.og \
            OGs/interface_match.og \
            OGs/internal-match.bits.og \
            OGs/internal.og \
            OGs/loanApprovalProcess.bits.og \
            OGs/loanApprovalProcess.og \
            OGs/myCoffee.bits.og \
            OGs/myCoffee.og \
            OGs/phcontrol3.unf.og \
            OGs/phcontrol.bits.og \
            OGs/PO.bits.og \
            OGs/PO.og \
            OGs/PO-pv.og \
            OGs/QR.bits.og \
            OGs/QR.og \
            OGs/RE.bits.og \
            OGs/RE.og \
            OGs/RS.bits.og \
            OGs/RS.og \
            OGs/SD.bits.og \
            OGs/SD.og \
            OGs/simple.og \
            OGs/simple_smaller_interface.og \
            OGs/TR.bits.og \
            OGs/TR.og \
            OGs/TR-pv.og \
            Services/AP.owfn \
            Services/AP-partner.owfn \
            Services/auctionService.owfn \
            Services/auctionService-partner.owfn \
            Services/BH.owfn \
            Services/BH-partner.owfn \
            Services/CN.owfn \
            Services/CN-partner.owfn \
            Services/DG.owfn \
            Services/DG-partner.owfn \
            Services/interface-partner.owfn \
            Services/internal_match.owfn \
            Services/internal_match-partner.owfn \
            Services/loanApprovalProcess.owfn \
            Services/loanApprovalProcess-partner.owfn \
            Services/myCoffee.owfn \
            Services/myCoffee-partner.owfn \
            Services/phcontrol3.owfn \
            Services/phcontrol3.unf.owfn \
            Services/phcontrol3.unf-partner.owfn \
            Services/PO.owfn \
            Services/PO-partner.owfn \
            Services/QR.owfn \
            Services/QR-partner.owfn \
            Services/RE.owfn \
            Services/RE-partner.owfn \
            Services/RS.owfn \
            Services/RS-partner.owfn \
            Services/SD.owfn \
            Services/SD-partner.owfn \
            Services/T2.owfn \
            Services/TR.owfn \
            Services/TR-partner.owfn


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
NO_COVER_FILES = */libs/* */*-lexic*.cc */*-syntax*.cc */cmdline.* */verbose.* */Output.*


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
