#############################################################################
# Copyright 2005, 2006, 2007 Niels Lohmann                                  #
#                                                                           #
# This file is part of GNU BPEL2oWFN.                                       #
#                                                                           #
# GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  #
# under the terms of the GNU General Public License as published by the     #
# Free Software Foundation; either version 2 of the License, or (at your    #
# option) any later version.                                                #
#                                                                           #
# GNU BPEL2oWFN is distributed in the hope that it will be useful, but      #
# WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  #
# Public License for more details.                                          #
#                                                                           #
# You should have received a copy of the GNU General Public License along   #
# with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  #
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. #
#############################################################################

# File last changed $Date: 2007/04/20 10:33:12 $ by $Author: nielslohmann $.
# Version $Revision: 1.19 $

TESTS = test1 test2 test3 test4 test5

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = $(BPEL_PROCESSES:.bpel=.info) \
	     $(BPEL_PROCESSES:.bpel=.lola) \
	     $(BPEL_PROCESSES:.bpel=.owfn) \
	     $(BPEL_PROCESSES:.bpel=.dot) \
	     $(BPEL_PROCESSES:.bpel=.xml) \
	     $(BPEL_PROCESSES:.bpel=.ast) \
             *.lola \
             *.task \
	     *.xml \
	     *.dot \
	     *.owfn \
	     *.png \
	     bpel2owfn.exe.stackdump \
	     core
	     
EXTRA_DIST = $(TESTS) \
	     testfiles \
	     sa_tests \
	     consistency
