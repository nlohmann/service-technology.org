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
TESTFILES = OrderingServiceProcess.bpel auction.bpel auction.wsdl auctionService.bpel auctionServiceInterface.wsdl chor_paper_agency.bpel chor_paper_airline.bpel chor_paper_topology1.xml chor_paper_topology10.xml chor_paper_traveler.bpel coffee.bpel customer1.bpel customer2.bpel customer3.bpel error.bpel loan.bpel loan.wsdl loanApprovalProcess.bpel loanServicePT.wsdl orderingLT.wsdl orderingPT.wsdl orderingProperties.wsdl purchase.bpel purchase.wsdl purchaseOrderProcess.bpel purchaseOrderProcess.dot purchaseOrderProcess.wsdl sa00002.bpel sa00002.wsdl sa00003.bpel sa00005.bpel sa00005.wsdl sa00006.bpel sa00007.bpel sa00008.bpel sa00015.bpel sa00016.bpel sa00017.bpel sa00018.bpel sa00023.bpel sa00024.bpel sa00025.bpel sa00032.bpel sa00035.bpel sa00036.bpel sa00037.bpel sa00044.bpel sa00048.bpel sa00048.wsdl sa00051.bpel sa00052.bpel sa00055.bpel sa00056.bpel sa00057.bpel sa00058.bpel sa00058.wsdl sa00059.bpel sa00060.bpel sa00061.bpel sa00062.bpel sa00063.bpel sa00064.bpel sa00065.bpel sa00066.bpel sa00067.bpel sa00068.bpel sa00069.bpel sa00070.bpel sa00071.bpel sa00072.bpel sa00073.bpel sa00074.bpel sa00075.bpel sa00076.bpel sa00077.bpel sa00078.bpel sa00079.bpel sa00080.bpel sa00081.bpel sa00082.bpel sa00083.bpel sa00084.bpel sa00087.bpel sa00087.wsdl sa00088.bpel sa00091.bpel sa00092.bpel sa00093.bpel shipping.bpel shippingLT.wsdl shippingPT.wsdl shippingProperties.wsdl shippingService.bpel


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
NO_COVER_FILES = */libs/* */cmdline.c */verbose.* \
                 */ast-config.cc */ast-grammar.cc */ast-system-k.cc \
                 */ast-system-k.h */ast-system-rk.cc */ast-system-rk.h \
                 */ast-system-unpk.cc */ast-system-unpk.h \
                 */frontend-lexer.cc \
                 */frontend-parser-chor.cc */frontend-parser-wsdl.cc \
                 */frontend-parser.cc */petrinet-patterns-basic.cc \
                 */petrinet-patterns-handlers.cc \
                 */petrinet-patterns-links.cc \
                 */petrinet-patterns-scopes.cc \
                 */petrinet-patterns-shortcuts.cc \
                 */petrinet-patterns-structured.cc


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests
