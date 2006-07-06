############################################################################
# Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert     #
#                                                                          #
# This file is part of BPEL2oWFN.                                          #
#                                                                          #
# BPEL2oWFN is free software; you can redistribute it and/or modify it     #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51  #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

# File last changed $Date: 2006/07/06 14:52:34 $ by $Author: nlohmann $.
# Version $Revision: 1.7 $

TESTS = test2 test3
#TESTS = test1 test2 test3 test4

TESTSUITE = syntax_test_suite/run_root_suite \
	    syntax_test_suite/process/run_test_suite \
	    syntax_test_suite/process/attribute/1.bpel \
	    syntax_test_suite/process/attribute/2.bpel \
	    syntax_test_suite/process/attribute/3.bpel \
	    syntax_test_suite/process/attribute/4.bpel \
	    syntax_test_suite/process/attribute/5.bpel \
	    syntax_test_suite/process/value/1.bpel \
	    syntax_test_suite/process/value/2.bpel \
	    syntax_test_suite/process/value/3.bpel \
	    syntax_test_suite/process/value/4.bpel \
	    syntax_test_suite/process/value/5.bpel \
	    syntax_test_suite/process/value/6.bpel \
	    syntax_test_suite/process/value/7.bpel \
	    syntax_test_suite/process/value/8.bpel \
	    syntax_test_suite/process/value/9.bpel \
	    syntax_test_suite/process/value/10.bpel \
	    syntax_test_suite/process/value/11.bpel

BPEL_PROCESSES = error.bpel loan.bpel purchase.bpel shipping.bpel

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = $(BPEL_PROCESSES:.bpel=.info) \
	     $(BPEL_PROCESSES:.bpel=.lola) \
	     $(BPEL_PROCESSES:.bpel=.owfn) \
	     $(BPEL_PROCESSES:.bpel=.dot) \
	     $(BPEL_PROCESSES:.bpel=.xml) \
	     $(BPEL_PROCESSES:.bpel=.ast)
	     
EXTRA_DIST = $(TESTS) $(BPEL_PROCESSES) $(TESTSUITE)
