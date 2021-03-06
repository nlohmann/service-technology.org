AC_INIT([Stacia], 1.0, stacia@service-technology.org, stacia, http://service-technology.org/stacia)

AC_MSG_NOTICE([ ____  _             _       ])
AC_MSG_NOTICE([/ ___|| |_ __ _  ___(_) __ _ ])
AC_MSG_NOTICE([\___ \| __/ _` |/ __| |/ _` |])
AC_MSG_NOTICE([ ___) | || (_| | (__| | (_| |])
AC_MSG_NOTICE([|____/ \__\__,_|\___|_|\__,_|])
AC_MSG_NOTICE([Siphon Trap Analysis -- Conquer In Action])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for standard programs needed to compile
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
# AC_PATH_PROG(FIONA,       [fiona], not found)
# AC_PATH_PROG(MARLENE,     [marlene], not found)
# AC_PATH_PROG(MIA,         [mia], not found)
# AC_PATH_PROG(PETRI,       [petri], not found)
# AC_PATH_PROG(VALGRIND,    [valgrind], not found)
# AC_PATH_PROG(WENDY,         [wendy], not found)

# search for optional tools
AC_PROG_LIBTOOL

# check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use pipes -- expect bad performance]))

# check and set exact unsigned types
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
# AC_TYPE_SIZE_T
# AC_CHECK_SIZEOF([void*])

# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile libs/minisat/Makefile src/Makefile src/stacia.conf doc/Makefile
	doc/Doxyfile tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

