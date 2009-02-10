# test_init MUST be the first, and test_tidy MUST be the last test case
TESTS = test_init \
	test_phils \
	test_process \
	test_planner \
	test_choreography \
	test_umlprocess \
	test_unbounded \
	test_limited \
	test_tidy

EXTRA_DIST = nets $(TESTS)

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests MAKE=$(MAKE)
