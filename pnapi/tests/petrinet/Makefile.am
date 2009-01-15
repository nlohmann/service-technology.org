TESTS = test_construction test_operators test_merging

check_PROGRAMS = test_construction test_operators test_merging

test_construction_SOURCES = test_construction.cc
test_operators_SOURCES    = test_operators.cc
test_merging_SOURCES      = test_merging.cc

# clean additional (generated) files
#MAINTAINERCLEANFILES = test_parser.net1.owfn test_parser.net2.owfn

# include test and api sources and libs
AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a
