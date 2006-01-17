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

# File last changed $Date: 2006/01/17 13:03:42 $ by $Author: nlohmann $.
# Version $Revision: 1.5 $

dnl Process this file with autoconf to produce a configure script.
AC_INIT(BPEL2oWFN, 1.1, nlohmann@informatik.hu-berlin.de, bpel2owfn)

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/bpel2owfn.h)

AC_LANG([C++])

AM_INIT_AUTOMAKE

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LEX

if test "$LEX" != "flex"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Flex. For details, read section `Needed Programs' in file README.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)

if test "$YACC" = "not found"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Bison. For details, read section `Needed Programs' in file README.])
fi

AC_CHECK_PROG(KC, kc++, kc++, not found)

if test "$KC" = "not found"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Kimwitu++. For details, read section `Needed Programs' in file README.])
fi

dnl no optimization flag here as it takes too much time
CXXFLAGS=-g

AC_OUTPUT(Makefile \
	  doc/Makefile \
	  src/Makefile \
	  tests/Makefile
	  )
