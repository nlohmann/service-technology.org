AC_INIT(Mia, 1.2-unreleased, mia@service-technology.org, mia, http://service-technology.org/mia)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for programs needed to compile
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_GREP
AC_PROG_AWK

# check for additional programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# export tools needed to determine memory consumption
AC_DEFINE_UNQUOTED(TOOL_AWK, "${AWK}", [awk])
AC_DEFINE_UNQUOTED(TOOL_GREP, "${GREP}", [grep])

# check and set exact unsigned types
AC_TYPE_UINT8_T

# set up autotest
AM_MISSING_PROG(AUTOM4TE, autom4te)
AC_CONFIG_TESTDIR(tests)

# store the SVN revision number
AC_DEFINE_UNQUOTED(VERSION_SVN, "`svnversion -n 2> /dev/null`", [The SVN revision.])

# allow the configure script to control assertions
AC_HEADER_ASSERT

# check for required tools
AC_PATH_PROG(LOLA_FULL, [lola-full], [])
AC_PATH_PROG(LOLA_FULL1, [lola-full1], [${LOLA_FULL}])
AC_PATH_PROG(WENDY, [wendy], [])

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))
AC_CHECK_FUNCS([mkstemp vfprintf],,AC_MSG_WARN([cannot use required functions]))

# check for optional tools needed by "make check"
AC_PATH_PROG(VALGRIND, [valgrind], not found)

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile src/mia.conf doc/Doxyfile
	doc/Makefile man/Makefile tests/Makefile tests/package.m4
	maintainer/Makefile maintainer/debian/changelog.template
	maintainer/debian/info maintainer/debian/README.Debian.template
	maintainer/debian/rules maintainer/macports/Portfile])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

# display a warning in case a required tool has not been found
if test "${LOLA_FULL}" = "" -o "${WENDY}" = ""; then
  echo
  echo "WARNING: Not all required tools have been found. Please read file"
  echo "         REQUIREMENTS and edit the file src/mia.conf before"
  echo "         using or installing Mia."
  echo
fi
