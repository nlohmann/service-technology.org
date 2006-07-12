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

# File last changed $Date: 2006/07/12 11:52:41 $ by $Author: nlohmann $.
# Version $Revision: 1.8 $

#TESTS = #test1
TESTS = test2 test3

BPEL_PROCESSES = error.bpel loan.bpel purchase.bpel shipping.bpel

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = $(BPEL_PROCESSES:.bpel=.info) \
	     $(BPEL_PROCESSES:.bpel=.lola) \
	     $(BPEL_PROCESSES:.bpel=.owfn) \
	     $(BPEL_PROCESSES:.bpel=.dot) \
	     $(BPEL_PROCESSES:.bpel=.xml) \
	     $(BPEL_PROCESSES:.bpel=.ast)
	     
EXTRA_DIST = $(TESTS) $(BPEL_PROCESSES)
