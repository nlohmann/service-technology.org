TESTS = test_construction test_operators test_merging test_output test_input

check_PROGRAMS = test_construction test_operators test_merging test_output test_input

test_construction_SOURCES = test_construction.cc
test_operators_SOURCES    = test_operators.cc
test_merging_SOURCES      = test_merging.cc
test_output_SOURCES       = test_output.cc
test_input_SOURCES        = test_input.cc

# include test and api sources and libs
AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a
