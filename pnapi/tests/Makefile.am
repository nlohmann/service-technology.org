TESTS = test_mergePlaces test_mergeTransitions test_mirror \
		test_removeTransition test_loop_final_state \
		test_produce \
		test_find_get_rename test_assignmentoperator \
		test_copyconstructor test_makeChannelsInternal \
		test_makeInnerStructure test_reduce_1 \
		test_reduce_2 test_reduce_3 test_reduce_4 \
		test_reduce_5 test_reduce_6 test_reduce_7

check_PROGRAMS =  test_mergePlaces test_mergeTransitions test_mirror \
			test_removeTransition test_loop_final_state \
			test_produce \
			test_find_get_rename test_assignmentoperator \
			test_copyconstructor test_makeChannelsInternal \
			test_makeInnerStructure test_reduce_1 \
			test_reduce_2 test_reduce_3 test_reduce_4 \
			test_reduce_5 test_reduce_6 test_reduce_7

test_mergePlaces_SOURCES = test_mergePlaces.cc net_gen.h net_gen.cc
test_mergeTransitions_SOURCES = test_mergeTransitions.cc net_gen.h net_gen.cc
test_mirror_SOURCES = test_mirror.cc net_gen.h net_gen.cc
test_removeTransition_SOURCES = test_removeTransition.cc net_gen.h net_gen.cc
test_loop_final_state_SOURCES = test_loop_final_state.cc net_gen.h net_gen.cc
test_produce_SOURCES = test_produce.cc
test_find_get_rename_SOURCES = test_find_get_rename.cc
test_assignmentoperator_SOURCES = test_assignmentoperator.cc
test_copyconstructor_SOURCES = test_copyconstructor.cc
test_makeChannelsInternal_SOURCES = test_makeChannelsInternal.cc
test_makeInnerStructure_SOURCES = test_makeInnerStructure.cc
test_reduce_1_SOURCES = test_reduce_1.cc
test_reduce_2_SOURCES = test_reduce_2.cc
test_reduce_3_SOURCES = test_reduce_3.cc
test_reduce_4_SOURCES = test_reduce_4.cc
test_reduce_5_SOURCES = test_reduce_5.cc
test_reduce_6_SOURCES = test_reduce_6.cc
test_reduce_7_SOURCES = test_reduce_7.cc

LDADD = $(top_builddir)/src/lib_pnapi.a


##############################################################################
# all you need for code coverage
##############################################################################

# the lcov scripts
EXTRA_DIST = lcov/README lcov/gendesc lcov/genhtml \
	lcov/geninfo lcov/genpng lcov/lcov

# remove files create during code coverage test
CLEANFILES = -fr output $(COVERINFOS) *.gcno *.gcda

# path to the tools
LCOV    = lcov/lcov
GENHTML = lcov/genhtml
GENDESC = lcov/gendesc
GENPNG  = lcov/genpng

# use .info as suffix for cover information
SUFFIXES = .info

# for each test, create an .info file
COVERINFOS = $(TESTS:=.info)

# wrapper for the testscripts
%.info: %
	@$(LCOV) --zerocounters --quiet --directory $(top_srcdir)/src
	-@$(TESTS_ENVIRONMENT) $(top_srcdir)/tests/$<
	@$(LCOV) --capture --directory $(top_srcdir)/src --output-file $@ --test-name $<

# compile the code for code coverage
cover-prepare:
	$(MAKE) --directory=$(top_srcdir) clean
	$(MAKE) --directory=$(top_srcdir)/src all \
	  CXXFLAGS="-fprofile-arcs -ftest-coverage" \
	  CFLAGS="-fprofile-arcs -ftest-coverage" \
	  LDFLAGS="-lgcov"

# create a HTML table from the results
cover:
	$(MAKE) $(COVERINFOS) \
	  CXXFLAGS="-fprofile-arcs -ftest-coverage" \
	  CFLAGS="-fprofile-arcs -ftest-coverage" \
	  LDFLAGS="-lgcov"
	$(GENHTML) $(COVERINFOS) \
	   --output-directory output --title "$(PACKAGE_STRING)" \
	   --show-details --legend
