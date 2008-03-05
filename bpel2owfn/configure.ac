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

# File last changed $Date: 2008/03/05 10:33:24 $ by $Author: gierds $.
# Version $Revision: 1.52 $

# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([GNU BPEL2oWFN],[2.0.4],[bpel2owfn-bug@gnu.org],[bpel2owfn])
AM_INIT_AUTOMAKE


# Set directories.
AC_CONFIG_SRCDIR(src/bpel2owfn.cc)
AC_CONFIG_HEADERS(src/bpel2owfn.h)


# The next line has to be un-commented before releasing version.
#AC_DEFINE(NDEBUG,1,[The release version does not need assert-calls.])


# Compute the canonical build-system type variable, build, and its three
# individual parts build_cpu, build_vendor, and build_os.
AC_CANONICAL_BUILD


# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC

# Unset compiler flags to avoid optimization.
CXXFLAGS=""
CFLAGS=""


# Checks for header files and functions.
AC_CHECK_HEADERS([windows.h libintl.h limits.h malloc.h stddef.h stdlib.h string.h strings.h unistd.h])
#AC_CHECK_FUNCS([pow])

AM_PROG_CC_C_O

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
	  gui/static_release.sh \
	  maintainer/fink/bpel2owfn.info \
	  maintainer/fink/create_fink_package.sh \
	  maintainer/rpm/Makefile \
	  maintainer/rpm/bpel2owfn.spec
	  ])

AC_OUTPUT

chmod 775 gui/static_release.sh
chmod 775 maintainer/fink/create_fink_package.sh

#AC_OUTPUT
