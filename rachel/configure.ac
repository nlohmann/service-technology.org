AC_INIT(Rachel, 1.15-unreleased, rachel@service-technology.org, rachel, http://service-technology.org/rachel)

AC_MSG_NOTICE([ ____            _          _ ])
AC_MSG_NOTICE([|  _ \ __ _  ___| |__   ___| |])
AC_MSG_NOTICE([| |_) / _` |/ __| '_ \ / _ \ |])
AC_MSG_NOTICE([|  _ < (_| | (__| | | |  __/ |])
AC_MSG_NOTICE([|_| \_\__,_|\___|_| |_|\___|_|])
AC_MSG_NOTICE([repairing deadlocking service choreographies])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(gnits)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_VERBOSE

# check for additional programs
AC_DEFINE_UNQUOTED(CONFIG_DOT, "\"$DOT\"", [whether dot was found by configure])

AC_PATH_PROG(DOT, [dot], not found)
if test "$DOT" = "not found"; then
  AC_MSG_WARN([Graphviz Dot was not found. See file README.])
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "not found", [Graphviz Dot was not found.])
else
  AC_DEFINE_UNQUOTED(CONFIG_DOT, "${DOT}", [Graphviz Dot can be used.])
fi
AC_SUBST(DOT)

AC_CONFIG_FILES([Makefile
                 doc/Doxyfile doc/Doxydetails doc/Makefile
                 exp/Makefile
                 man/Makefile
                 src/Makefile
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT
