TESTS = test_mergePlaces test_mergeTransitions test_mirror \
		test_removeTransition test_loop_final_state \
		test_produce \
		test_find_get_rename test_assignmentoperator \
		test_copyconstructor test_makeChannelsInternal \
		test_makeInnerStructure

check_PROGRAMS =  test_mergePlaces test_mergeTransitions test_mirror \
			test_removeTransition test_loop_final_state \
			test_produce \
			test_find_get_rename test_assignmentoperator \
			test_copyconstructor test_makeChannelsInternal \
			test_makeInnerStructure

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

LDADD = $(top_builddir)/src/lib_pnapi.a
