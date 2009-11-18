AC_INIT(Fiona GUI,0.01)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC

AM_MISSING_PROG(QMAKE,qmake)
AM_MISSING_PROG(MOC,moc)

# check for Qt libraries
AC_CHECK_LIB(QtCore,main)
AC_CHECK_LIB(QtGui,main)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
