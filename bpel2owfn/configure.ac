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

# File last changed $Date: 2006/07/12 10:55:07 $ by $Author: nlohmann $.
# Version $Revision: 1.15 $

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([BPEL2oWFN],[2.0],[nlohmann@informatik.hu-berlin.de],[bpel2owfn])
AC_REVISION($Revision: 1.15 $)
AM_INIT_AUTOMAKE


# Set directories.
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/bpel2owfn.h)


# The next line has to be un-commented to improve performance.
# AC_DEFINE(NDEBUG,1,[The release version does not need assert-calls.])


# Checks for C/C++ details.
AC_LANG([C++])
AC_C_INLINE
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_TYPE_SIZE_T


# Checks for header files and functions.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h unistd.h stddef.h])
AC_CHECK_FUNCS([pow])
AC_FUNC_REALLOC


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])


# Check if the compiler generators are there
AC_PROG_LEX
if test "$LEX" != "flex"; then
 AC_MSG_WARN([Flex was not found.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)
if test "$YACC" = "not found"; then
 AC_MSG_WARN([Bison was not found.])
fi

AC_CHECK_PROG(KC, kc++, kc++, not found)
if test "$KC" = "not found"; then
 AC_MSG_WARN([Kimwitu++ was not found.])
fi

AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
fi


# Add this parameters to debug.
#CXXFLAGS="-g -Wall"
CXXFLAGS=""


# Set files to generate.
AC_CONFIG_FILES([Makefile \
	  doc/Makefile \
	  src/Makefile \
	  tests/Makefile
	  ])

AC_OUTPUT
