TESTS = test_construction   \
	test_operators      \
	test_merging        \
	test_output         \
	test_input          \
	test_normalize      \
	test_markings       \
	test_produce

check_PROGRAMS = $(TESTS)

test_construction_SOURCES   = test_construction.cc
test_operators_SOURCES	    = test_operators.cc
test_merging_SOURCES        = test_merging.cc
test_output_SOURCES         = test_output.cc
test_input_SOURCES          = test_input.cc
test_normalize_SOURCES      = test_normalize.cc
test_markings_SOURCES	    = test_markings.cc
test_produce_SOURCES	    = test_produce.cc

EXTRA_DIST = $(TESTS) \
	test_input.net.owfn \
	test_produce.onlineshop.owfn \
	test_produce.onlineshop.constraint.owfn


AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.la


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
