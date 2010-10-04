AC_INIT([Domino], 0.9-unreleased, domino@service-technology.org, domino, http://service-technology.org/domino)


AC_MSG_NOTICE( ____                  _             )
AC_MSG_NOTICE(|  _ \  ___  _ __ ___ (_)_ __   ___  )
AC_MSG_NOTICE(| | | |/ _ \| '_ ` _ \| | '_ \ / _ \ )
AC_MSG_NOTICE(| |_| | (_) | | | | | | | | | | (_) |)
AC_MSG_NOTICE(|____/ \___/|_| |_| |_|_|_| |_|\___/ )
AC_MSG_NOTICE([Decomposition of business workflows into services])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_LOLA([1.14])
AC_ST_AUTOTEST
AC_ST_PNAPI([4.03])
AC_ST_VERBOSE

# search for optional tools needed by "make check"
# AC_PATH_PROG(FIONA,       [fiona], not found)
# AC_PATH_PROG(MARLENE,     [marlene], not found)
# AC_PATH_PROG(MIA,         [mia], not found)
# AC_PATH_PROG(PETRI,       [petri], not found)
AC_PATH_PROG(VALGRIND,    [valgrind], not found)
# AC_PATH_PROG(WENDY,         [wendy], not found)
AC_PATH_PROG(DIANE,         [diane], not found)


# check for required functions and die if they are not found
AC_CHECK_FUNCS([popen pclose],,AC_MSG_ERROR([cannot use required functions]))

# FreeBSD does not know log2()
# AC_CHECK_FUNCS([log2])

# check and set exact unsigned types
# AC_TYPE_UINT8_T
# AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_SIZE_T
# AC_CHECK_SIZEOF([void*])


# write files
AC_CONFIG_FILES([
	Makefile libs/pnapi/Makefile src/Makefile src/domino.conf doc/Makefile
	doc/Doxyfile tests/Makefile tests/package.m4 doc/Doxydetails])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

