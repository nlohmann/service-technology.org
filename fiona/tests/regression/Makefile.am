# list all testscripts
TESTS = bug11996.sh bug12101.sh bug12207.sh bug12239.sh bug12765.sh bug12829.sh bug12895.sh

# list those testscripts that are expected to fail
XFAIL_TESTS = bug12895.sh

# list the testscripts and testfiles
EXTRA_DIST = $(TESTS) \
             bug11996/a.owfn.og \
	     bug11996/b.owfn.og \
	     bug12101/myCoffee.owfn \
	     bug12207/coffee_extended.fixed.owfn.ig.og \
             bug12207/coffee_extended.owfn.ig.og \
             bug12239/coffee_extended-partner.normalized.owfn \
             bug12765/cabbababb.owfn \
             bug12829/b1-1.s00000035__s00000934.owfn \
             bug12829/b1.s00000035__s00000934.owfn \
             bug12895/non_empty_true_node3.owfn


#############################################################################
# everything below is generic and should not be changed
#############################################################################
# this wrapper script organizes the temp file generation
EXTRA_DIST += wrap.sh

# where to execute the tests (usually a temp folder)
TESTDIR = $${TMPDIR-/tmp}/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)/$(subdir)

# variables passed to the testscripts
TESTS_ENVIRONMENT = testdir=$(TESTDIR) testroot=$(abs_srcdir) PATH=$(abs_top_builddir)/src:$$PATH $(srcdir)/wrap.sh

# "make clean" should delete the result of the tests
CLEANFILES = -r $(TESTDIR)
