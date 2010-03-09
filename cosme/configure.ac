AC_INIT(Cosme, 0.7, cosme@service-technology.org, cosme, http://service-technology.org/cosme)

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE#(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(VALGRIND, [valgrind], not found)
AC_PATH_PROG(LOLA,     [lola-statespace], [])

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# check and set exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# write files
AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/wendy.conf before"
  echo "         using or installing Wendy."
  echo
fi
