TESTS           = test_construction   \
			      test_operators      \
			      test_merging        \
			      test_output         \
	 		      test_input          \
	 		      test_normalize


check_PROGRAMS  = test_construction   \
			      test_operators      \
			      test_merging        \
			      test_output         \
			      test_input          \
			      test_normalize


test_construction_SOURCES   = test_construction.cc
test_operators_SOURCES      = test_operators.cc
test_merging_SOURCES        = test_merging.cc
test_output_SOURCES         = test_output.cc
test_input_SOURCES          = test_input.cc
test_normalize_SOURCES      = test_normalize.cc


AM_CPPFLAGS = -I$(top_srcdir)/tests           -I$(top_srcdir)/src
LDADD       = $(top_builddir)/tests/libtest.a $(top_builddir)/src/libpnapi.a

DISTCLEANFILES = test_input.net1.owfn test_input.net2.owfn \ 
                 test_normalize.coffee_unnormal.owfn \ 
                 test_normalize.coffee_unnormal_normalized.owfn \ 
                 test_normalize.sequence_normal.owfn \ 
                 test_normalize.sequence_normal_normalized.owfn \ 
                 test_normalize.strange_unnormal.owfn \ 
                 test_normalize.strange_unnormal_normalized.owfn

