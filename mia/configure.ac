AC_INIT(Mia, 1.3-unreleased, mia@service-technology.org, mia, http://service-technology.org/mia)

AC_MSG_NOTICE([ __  __ _       ])
AC_MSG_NOTICE([|  \/  (_) __ _ ])
AC_MSG_NOTICE([| |\/| | |/ _` |])
AC_MSG_NOTICE([| |  | | | (_| |])
AC_MSG_NOTICE([|_|  |_|_|\__,_|])
AC_MSG_NOTICE([migrating instances of running services])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_PNAPI([4.02])
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(LOLA_STATESPACE, [lola-statespace], [])
AC_PATH_PROG(LOLA_STATESPACE1, [lola-statespace1], [${LOLA_STATESPACE}])
AC_PATH_PROG(WENDY, [wendy], [])
AC_PATH_PROG(VALGRIND, [valgrind], not found)

# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# check and set exact unsigned types
AC_TYPE_UINT8_T

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile src/mia.conf doc/Doxyfile
	doc/Makefile man/Makefile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA_STATESPACE}" = "" -o "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/mia.conf before"
  echo "         using or installing Mia."
  echo
fi
