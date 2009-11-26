# M4 macros to build Qt apps that use qscintilla2 with the autotools
# (Autoconf/Automake).
# This file is part of AutoTroll.
# Copyright (C) 2006  Benoit Sigoure <tsuna@lrde.epita.fr>
#
# AutoTroll is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
# USA.
#
# In addition, as a special exception, the copyright holders of AutoTroll
# give you unlimited permission to copy, distribute and modify the configure
# scripts that are the output of Autoconf when processing the macros of
# AutoTroll.  You need not follow the terms of the GNU General Public License
# when using or distributing such scripts, even though portions of the text of
# AutoTroll appear in them. The GNU General Public License (GPL) does govern
# all other use of the material that constitutes AutoTroll.
#
# This special exception to the GPL applies to versions of AutoTroll
# released by the copyright holders of AutoTroll.  Note that people who make
# modified versions of AutoTroll are not obligated to grant this special
# exception for their modified versions; it is their choice whether to do so.
# The GNU General Public License gives permission to release a modified version
# without this exception; this exception also makes it possible to release a
# modified version which carries forward this exception.

# This file relies on values (QT_*) defined by AutoTroll.
# Although it is not *required* to use AutoTroll along with this file, doing
# so provides better results since QScintilla is often located where Qt is.

# WITH_QSCINTILLA2()
# ------------------
# Check whether QsciScintilla2 is installed and usable.
# Defines $(QSCINTILLA2_LDFLAGS) which must be used before the Qt flags (if
# any).
AC_DEFUN([WITH_QSCINTILLA2],
[ AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_EXEEXT])

  AC_ARG_WITH([qscintilla2],
              [AS_HELP_STRING([--with-qscintilla2].
                 [LDFLAGS to use for qscintilla2 @<:@-lqscintilla2 + Same as Qt@:>@])],
              [QSCINTILLA2_LDFLAGS=$withval], [QSCINTILLA2_LDFLAGS='-lqscintilla2'])

  # FIXME: Can we use a macro from autoconf to make something similar?
  AC_CACHE_CHECK([for lib qscintilla2], [qt_cv_lqscintilla2],
  [qt_cv_lqscintilla2=no
  if mkdir conftest.dir && cd conftest.dir; then :; else
    AC_MSG_ERROR([Cannot mkdir conftest.dir or cd to that directory.])
  fi
  cat >conftest.cpp <<_ASEOF
#include <Qsci/qsciscintilla.h>
#ifdef main  // Temporary work-around for Windows: Qt #defines main qMain
# undef main // but for some reason *I* can't link Qt apps because the linker
#endif       // fails to find it. To be FIXME'd.
struct Test: public QsciScintilla
{
};

int main ()
{
  Test t;
}
_ASEOF
  cat >Makefile <<_ASEOF
CXX = $CXX
QT_DEFINES = $QT_DEFINES
QT_CPPFLAGS = $QT_CPPFLAGS
AM@&t@?_CPPFLAGS = $AM@&t@?_CPPFLAGS
CPPFLAGS = $CPPFLAGS
QT_CXXFLAGS = $QT_CXXFLAGS
AM@&t@?_CXXFLAGS = $AM@&t@?_CXXFLAGS
CXXFLAGS = $CXXFLAGS
QT_LDFLAGS = $QT_LDFLAGS
LDFLAGS = $LDFLAGS
QSCINTILLA2_LDFLAGS = $QSCINTILLA2_LDFLAGS
QT_LIBS = $QT_LIBS
LIBS = $LIBS

all: conftest$EXEEXT

conftest$EXEEXT: conftest.cpp
	$CXX $QT_CPPFLAGS $AM_CPPFLAGS $CPPFLAGS \
          $MY_QT_CXXFLAGS $AM_CXXFLAGS $CXXFLAGS \
          conftest.cpp -o conftest$EXEEXT \
          $QT_LDFLAGS $LDFLAGS \
          $QSCINTILLA2_LDFLAGS $QT_LIBS $LIBS
_ASEOF

  : ${MAKE=make}
  echo "$as_me:$LINENO: running $MAKE to check that qscintilla2 works:" \
    >&AS_MESSAGE_LOG_FD
  if $MAKE >&AS_MESSAGE_LOG_FD 2>&1;
  then
    if test x"$QT_LIBS" != x; then
      qt_cv_lqscintilla2="$QSCINTILLA2_LDFLAGS + Qt stuff"
    else
      qt_cv_lqscintilla2="$QSCINTILLA2_LDFLAGS"
    fi
  fi
  if test x"$qt_cv_lqscintilla2" = xno; then
    echo "$as_me:$LINENO: failed program was:" >&AS_MESSAGE_LOG_FD
    sed 's/^/| /' conftest.cpp >&AS_MESSAGE_LOG_FD
    echo "$as_me:$LINENO: tried to compile with the following Makefile:" \
      >&AS_MESSAGE_LOG_FD
    sed 's/^/| /' Makefile >&AS_MESSAGE_LOG_FD
  fi
  cd .. && rm -rf conftest.dir
  ])dnl End of AC_CACHE_CHECK for -lqscintilla2

  if test x"$qt_cv_lqscintilla2" = xno; then
    AC_MSG_ERROR([Cannot link with -lqscintilla2.])
  fi
  AC_SUBST([QSCINTILLA2_LDFLAGS])
])
