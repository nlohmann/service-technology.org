TESTS = bug11996.sh bug12101.sh bug12207.sh bug12239.sh bug12765.sh bug12829.sh bug12895.sh
XFAIL_TESTS = bug12895.sh

TESTS_ENVIRONMENT = testdir=$(srcdir) builddir=$(builddir) PATH=$(abs_top_builddir)/src:$$PATH

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

CLEANFILES = bug11996/a.owfn_X_b.owfn.og \
             bug11996/a.owfn_X_b.owfn.out \
             bug12101/myCoffee.ig.og \
             bug12207/bug12207/coffee_extended.fixed.owfn.ig.og.out \
             bug12895/non_empty_true_node3.pv.owfn

dist-hook:
	$(MAKE) $(AM_MAKEFLAGS) clean
