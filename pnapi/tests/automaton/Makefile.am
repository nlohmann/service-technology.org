#SUBDIRS = . #input

TESTS = test_construction \
	test_statemachine

check_PROGRAMS = ${TESTS}

test_construction_SOURCES = test_construction.cc
test_statemachine_SOURCES = test_statemachine.cc

# include test and api sources and libs
AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.la

EXTRA_DIST = $(TESTS) \
             sequence_normal.net.owfn \
             sequence_unnormal.net.owfn \
             strange_net.net.owfn \
	     test_statemachine.net.owfn


#############################################################################
# everything below is generic and should not be changed
#############################################################################
# this wrapper script organizes the temp file generation
EXTRA_DIST += ../wrap.sh

# where to execute the tests (usually a temp folder inside the build dir)
TESTDIR = $(abs_builddir)/tmp

# variables passed to the testscripts
TESTS_ENVIRONMENT = testdir=$(TESTDIR) testroot=$(abs_srcdir) PATH=$(abs_top_builddir)/utils:$$PATH $(srcdir)/../wrap.sh

# "make clean" should delete the result of the tests
CLEANFILES = -r $(TESTDIR)

