TESTS           = copy_owfns.sh \
		  test_unused_status_places \
		  test_suspicious_transitions \
		  test_dead_nodes \
		  test_initially_marked \
		  test_starke_3p \
		  test_starke_3t \
		  test_starke_4 \
		  test_starke_5 \
		  test_starke_6 \
		  test_starke_7 \
		  test_starke_8 \
		  test_starke_9 \
		  test_identical_places \
		  test_identical_transitions \
		  test_series_places \
		  test_series_transitions \
		  test_self_loop_places \
		  test_self_loop_transitions \
		  test_equal_places 


check_PROGRAMS  = test_unused_status_places \
		  test_suspicious_transitions \
		  test_dead_nodes \
		  test_initially_marked \
		  test_starke_3p \
		  test_starke_3t \
		  test_starke_4 \
		  test_starke_5 \
		  test_starke_6 \
		  test_starke_7 \
		  test_starke_8 \
		  test_starke_9 \
		  test_identical_places \
		  test_identical_transitions \
		  test_series_places \
		  test_series_transitions \
		  test_self_loop_places \
		  test_self_loop_transitions \
		  test_equal_places 

test_unused_status_places_SOURCES = test_unused_status_places.cc
test_suspicious_transitions_SOURCES = test_suspicious_transitions.cc
test_dead_nodes_SOURCES = test_dead_nodes.cc 
test_initially_marked_SOURCES = test_initially_marked.cc 
test_starke_3p_SOURCES = test_starke_3p.cc 
test_starke_3t_SOURCES = test_starke_3t.cc 
test_starke_4_SOURCES = test_starke_4.cc
test_starke_5_SOURCES = test_starke_5.cc
test_starke_6_SOURCES = test_starke_6.cc
test_starke_7_SOURCES = test_starke_7.cc
test_starke_8_SOURCES = test_starke_8.cc
test_starke_9_SOURCES = test_starke_9.cc
test_identical_places_SOURCES = test_identical_places.cc 
test_identical_transitions_SOURCES = test_identical_transitions.cc
test_series_places_SOURCES = test_series_places.cc
test_series_transitions_SOURCES = test_series_transitions.cc
test_self_loop_places_SOURCES = test_self_loop_places.cc
test_self_loop_transitions_SOURCES = test_self_loop_transitions.cc
test_equal_places_SOURCES = test_equal_places.cc 


AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a

TESTS_ENVIRONMENT = srcdir=$(srcdir) builddir=$(builddir)

EXTRA_DIST = copy_owfns.sh \
	     owfn/test_dead_nodes.owfn

DISTCLEANFILES = test_dead_nodes.owfn
