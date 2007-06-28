#############################################################################
# GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models
#
# Copyright (C) 2006, 2007  Niels Lohmann,
#                           Christian Gierds, and
#                           Martin Znamirowski
# Copyright (C) 2005        Niels Lohmann and
#			    Christian Gierds
#
# GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.
#
# GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# GNU BPEL2oWFN; if not, see http://www.gnu.org/licenses or write to the Free
# Software Foundation,Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA.
#############################################################################

# File last changed $Date: 2007/06/28 07:38:18 $ by $Author: nielslohmann $.
# Version $Revision: 1.21 $

TESTS = test1 test2 test3 test4 test5 test6

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = *.lola \
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
	     consistency \
	     bpel4chor
