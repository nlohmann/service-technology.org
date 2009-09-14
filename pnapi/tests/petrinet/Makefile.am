TESTS = test_merging        \
	test_normalize

check_PROGRAMS = $(TESTS)

test_merging_SOURCES        = test_merging.cc
test_normalize_SOURCES      = test_normalize.cc

EXTRA_DIST = $(TESTS)


AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.la
AM_CXXFLAGS = @configured_CXXFLAGS@
AM_CFLAGS = @configured_CFLAGS@


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

