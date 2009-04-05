#SUBDIRS = . #input

TESTS = test_construction \
	test_statemachine

check_PROGRAMS = test_construction \
		 test_statemachine

test_construction_SOURCES = test_construction.cc
test_statemachine_SOURCES = test_statemachine.cc

# include test and api sources and libs
AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a

EXTRA_DIST = sequence_normal.owfn sequence_unnormal.owfn strange_net.owfn \
	     test_statemachine.net.owfn
