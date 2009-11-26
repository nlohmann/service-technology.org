#################
# Package Setup #
#################

AC_INIT([Fiona-GUI], [0.02], [stephan.mennicke@uni-rostock.de], [fiona-gui])

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#######################
# Tool/Compiler Setup #
#######################

# check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC

# AutoTroll with Qt
AT_WITH_QT

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
