TESTS = bug12829.sh bug12765.sh bug12895.sh
XFAIL_TESTS = bug12895.sh

TESTS_ENVIRONMENT = testdir=$(srcdir) PATH=$(abs_top_builddir)/src:$$PATH

CLEANFILES = bug12895/non_empty_true_node3.pv.owfn

EXTRA_DIST = $(TESTS) \
             bug12765/cabbababb.owfn \
             bug12829/b1-1.s00000035__s00000934.owfn \
             bug12829/b1.s00000035__s00000934.owfn \
             bug12895/non_empty_true_node3.owfn

dist-hook:
	$(MAKE) $(AM_MAKEFLAGS) clean
