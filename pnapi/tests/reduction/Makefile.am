TESTS           = test_dead_nodes.sh \
                  test_identical_places.sh \
                  test_identical_transitions.sh \
                  test_series_places.sh \
                  test_series_transitions.sh \
                  test_self_loop_places.sh \
                  test_self_loop_transitions.sh \
                  test_starke_3p.sh \
                  test_starke_3t.sh \
                  test_starke_4.sh \
                  test_starke_5.sh \
                  test_starke_6.sh \
                  test_starke_7.sh \
                  test_starke_8.sh \
                  test_starke_9.sh 

EXTRA_DIST = $(TESTS) \
             test_dead_nodes/test_dead_nodes.net.owfn \
             test_identical_places/test_identical_places.net.owfn \
             test_identical_transitions/test_identical_transitions.net.owfn \
             test_series_places/test_series_places.net.owfn \
             test_series_transitions/test_series_transitions.net.owfn \
             test_series_transitions_keepnormal/test_series_transitions_keepnormal.net.owfn \
             test_self_loop_places/test_self_loop_places.net.owfn \
             test_self_loop_transitions/test_self_loop_transitions.net.owfn \
             test_starke_3p/test_starke_3p.net.owfn \
             test_starke_3t/test_starke_3t.net.owfn \
             test_starke_4/test_starke_4.net.owfn \
             test_starke_5/test_starke_5.net.owfn \
             test_starke_6/test_starke_6.net.owfn \
             test_starke_7/test_starke_7.net.owfn \
             test_starke_8/test_starke_8.net.owfn \
             test_starke_9/test_starke_9.net.owfn 


#############################################################################
# everything below is generic and should not be changed
#############################################################################
# this wrapper script organizes the temp file generation
EXTRA_DIST += ./wrap.sh

# where to execute the tests (usually a temp folder inside the build dir)
TESTDIR = $(abs_builddir)/tmp

# variables passed to the testscripts
TESTS_ENVIRONMENT = testdir=$(TESTDIR) testroot=$(abs_srcdir) \
                    GREP=$(GREP) \
                    PATH=$(abs_top_builddir)/utils:$$PATH $(srcdir)/wrap.sh 

# "make clean" should delete the result of the tests
CLEANFILES = -r $(TESTDIR)
