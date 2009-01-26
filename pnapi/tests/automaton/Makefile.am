TESTS = test_construction 

check_PROGRAMS = test_construction 

test_construction_SOURCES = test_construction.cc

# include test and api sources and libs
AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a
