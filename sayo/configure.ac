AC_INIT([Sayo], 1.2, sayo@service-technology.org, sayo, http://service-technology.org/sayo)

AC_MSG_NOTICE([     ____                         ])
AC_MSG_NOTICE([    / ___|  __ _ _   _  ___       ])
AC_MSG_NOTICE([    \___ \ / _` | | | |/ _ \      ])
AC_MSG_NOTICE([     ___) | (_| | |_| | (_) |     ])
AC_MSG_NOTICE([    |____/ \__,_|\__, |\___/      ])
AC_MSG_NOTICE([                 |___/            ])
AC_MSG_NOTICE([generating public view from bit-og])
AC_MSG_NOTICE([${PACKAGE_URL}])
AC_MSG_NOTICE([version ${PACKAGE_VERSION}])
AC_MSG_NOTICE()

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

# use the service-technology.org macros
AC_ST_BASIC
AC_ST_AUTOTEST
AC_ST_VERBOSE

# search for optional tools needed by "make check"
AC_PATH_PROG(FIONA, [fiona], not found)
AC_PATH_PROG(PETRI, [petri], not found)
AC_PATH_PROG(WENDY, [wendy], not found)
AC_PATH_PROG(WENDY2FIONA, [wendy2fiona], not found)

# write files
AC_CONFIG_FILES([Makefile
                 doc/Makefile doc/Doxyfile
                 doc/Doxydetails src/Makefile
                 tests/Makefile tests/package.m4])
AC_CONFIG_FILES([src/publicview.sh], [chmod +x src/publicview.sh])
AC_CONFIG_FILES([tests/cover.sh], [chmod +x tests/cover.sh])
AC_OUTPUT

