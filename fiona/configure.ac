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

# File last changed $Date: 2006-07-06 13:37:10 $ by $Author: bretschn $.
# Version $Revision: 1.4 $

# Process this file with autoconf to produce a configure script.
AC_INIT(Fiona, 1.0, weinberg@informatik.hu-berlin.de, fiona)
AC_REVISION($Revision: 1.4 $)

AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_HEADERS(src/fiona.h)

AM_INIT_AUTOMAKE

AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL


# check for getopt
# AC_CHECK_HEADER(getopt.h)


# check if the compiler generators are there
AC_PROG_LEX
if test "$LEX" != "flex"; then
 AC_MSG_WARN([Flex was not found.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)
if test "$YACC" = "not found"; then
 AC_MSG_WARN([Bison was not found.])
fi

# Check for --enable-64bit parameter to enable 64bit compilation
AC_ARG_ENABLE(64bit,
  [  --enable-64bit  build for 64bit architecture [default=no]],
  enable_64bit="yes", enable_64bit="no")
AM_CONDITIONAL(ENABLE_64BIT, test "$enable_64bit" = "yes")

# no optimization flag here as it takes too much time
CXXFLAGS=-g

AC_OUTPUT(Makefile \
	  src/Makefile 
	  )

# Print summary of compile settings
echo
echo "--------- compile settings ---------"
echo "compile for 64bit...: $enable_64bit"
echo "------------------------------------"
echo
