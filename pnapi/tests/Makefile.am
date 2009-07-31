SUBDIRS = . petrinet

check_LIBRARIES = libtest.a

libtest_a_SOURCES = test.h test.cc


# needed includes
AM_CPPFLAGS = -I$(top_srcdir)/src


# AUTOTEST
TESTSUITE = $(builddir)/testsuite
DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)
EXTRA_DIST = testsuite.at versions.sh \
             testfiles/test_input.owfn \
             testfiles/syntax_example1.owfn \
             testfiles/syntax_example2.owfn \
             testfiles/syntax_example3.owfn \
             testfiles/syntax_example4.owfn \
	     testfiles/bug13579.owfn \
             testfiles/test_input.lola \
             testfiles/test_dead_nodes.net.owfn \
             testfiles/test_identical_places.net.owfn \
             testfiles/test_identical_transitions.net.owfn \
             testfiles/test_self_loop_places.net.owfn \
             testfiles/test_self_loop_transitions.net.owfn \
             testfiles/test_series_places.net.owfn \
             testfiles/test_series_transitions.net.owfn \
             testfiles/test_series_transitions_keepnormal.net.owfn \
             testfiles/test_starke_3p.net.owfn \
             testfiles/test_starke_3t.net.owfn \
             testfiles/test_starke_4.net.owfn \
             testfiles/test_starke_5.net.owfn \
             testfiles/test_starke_6.net.owfn \
             testfiles/test_starke_7.net.owfn \
             testfiles/test_starke_8.net.owfn \
             testfiles/test_starke_9.net.owfn \
             testfiles/test_criteria.noworkflownet.owfn \
             testfiles/test_criteria.workflownet.owfn \
             testfiles/test_criteria.nofreechoice.owfn \
             testfiles/test_criteria.freechoice.owfn \
             testfiles/test_construction.sequence_normal.net.owfn \
             testfiles/test_construction.sequence_unnormal.net.owfn \
             testfiles/test_construction.strange_net.net.owfn \
             testfiles/test_myCoffee.sa testfiles/test_myCoffee-sm.owfn testfiles/test_myCoffee-syn.owfn \
	     testfiles/syntax_example.owfn \
	     testfiles/syntax_example_new.owfn \
	     testfiles/syntax_example_new_short.owfn \
	     testfiles/syntax_formula.owfn \
	     testfiles/syntax_max_occurrence.owfn 


check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
