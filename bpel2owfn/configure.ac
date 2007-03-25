#############################################################################
# Copyright (C) 2007       Niels Lohmann                                    #
# Copyright (C) 2005, 2006 Niels Lohmann, Christian Gierds                  #
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

# File last changed $Date: 2007/03/25 10:19:37 $ by $Author: nielslohmann $.
# Version $Revision: 1.40 $

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([GNU BPEL2oWFN],[2.0.0],[bpel2owfn-bug@gnu.org],[bpel2owfn])
AM_INIT_AUTOMAKE


# Set directories.
AC_CONFIG_SRCDIR(src/bpel2owfn.cc)
AC_CONFIG_HEADERS(src/bpel2owfn.h)


# The next line has to be un-commented before releasing version.
#AC_DEFINE(NDEBUG,1,[The release version does not need assert-calls.])


# Compute the canonical build-system type variable, build, and its three
# individual parts build_cpu, build_vendor, and build_os.
AC_CANONICAL_BUILD


# Checks for C/C++ details.
AC_PROG_CXX
AC_PROG_CC


# Unset compiler flags to avoid optimization.
CXXFLAGS=""
CFLAGS=""


# Checks for header files and functions.
AC_FUNC_ALLOCA
AC_HEADER_STDC

AC_CHECK_HEADERS([libintl.h malloc.h stddef.h stdlib.h string.h strings.h unistd.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow])


# Check for optional programs.
AC_PROG_YACC
AC_PROG_LEX

AC_CHECK_PROG(KC, kc++, kc++, not found)
if test "$KC" = "not found"; then
 AC_MSG_WARN([Kimwitu++ was not found. You will not be able to complile BPEL2oWFN from scratch.])
fi

AC_CHECK_PROG(DOT, dot, dot, not found)
if test "$DOT" = "not found"; then
 AC_MSG_WARN([Graphviz dot was not found.])
 AC_DEFINE(HAVE_DOT, 0, [Graphviz dot can not be used on this system.])
else
 AC_DEFINE(HAVE_DOT, 1, [Graphviz dot can be used on this system.])
fi


# Set files to generate.
AC_CONFIG_FILES([Makefile \
	  doc/Makefile \
	  src/Makefile \
	  src/Doxyfile \
	  tests/Makefile \
	  rpm/Makefile \
	  rpm/bpel2owfn.spec
	  ])


AC_OUTPUT
