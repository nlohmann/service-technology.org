dnl Process this file with autoconf to produce a configure script.
AC_INIT(BPEL2oWFN, 1.0, nlohmann@informatik.hu-berlin.de, bpel2owfn)

AC_CONFIG_SRCDIR(src/main.cc)

AC_LANG([C++])

AM_INIT_AUTOMAKE

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LEX

if test "$LEX" != "flex"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Flex. For details, read section `Needed Programs' in file README.])
fi

AC_CHECK_PROG(YACC, bison, bison, not found)

if test "$YACC" = "not found"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Bison. For details, read section `Needed Programs' in file README.])
fi

AC_CHECK_PROG(KC, kc++, kc++, not found)

if test "$KC" = "not found"; then
 AC_MSG_FAILURE([BPEL2oWFN cannot be compiled without Kimwitu++. For details, read section `Needed Programs' in file README.])
fi

dnl no optimization flag here as it takes too much time
CXXFLAGS=-g

AC_OUTPUT(Makefile \
	  doc/Makefile \
	  src/Makefile \
	  tests/Makefile
	  )
