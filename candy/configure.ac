# RICH überprüft autoconf version
AC_PREREQ(2.59)

# RICH initiale Daten für autoconf
AC_INIT([Candy], [0.1], [richard.mueller@service-technology.org])

# RICH überprüft ob bestimmte Dateien vorhanden sind (Stichprobe)
AC_CONFIG_SRCDIR([src/main.cc])

# RICH Testergebnisse werden als #defines in header geschrieben
AC_CONFIG_HEADERS([src/config.h])

# RICH automake initialisieren
AM_INIT_AUTOMAKE

# RICH checks for programs
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

# RICH checks for programs
AM_MISSING_PROG(GENGETOPT, gengetopt)
AM_MISSING_PROG(HELP2MAN, help2man)

# RICH needed for the Petri Net API
AC_DEFINE_UNQUOTED(CONFIG_PETRIFY, "not found", [Petrify is not used.])

# RICH checks whether to enable assertions in the style of assert.h
AC_HEADER_ASSERT

# RICH check for functions
AC_CHECK_FUNCS([popen pclose],,AC_MSG_WARN([cannot use popen or pclose]))

# RICH brauch ich nicht
# check for exact unsigned types
#AC_TYPE_UINT8_T
#AC_TYPE_UINT16_T
#AC_TYPE_UINT32_T

# RICH zu konfigurierende Dateien
AC_CONFIG_FILES([Makefile libs/pnapi/Makefile src/Makefile doc/Doxyfile man/Makefile])

# RICH los gehts
AC_OUTPUT
