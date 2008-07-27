TESTS = test1 test2 test3 test4

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:$(top_builddir)/src:$$PATH

CLEANFILES = $(top_builddir)/tests/*.bpel
	     
EXTRA_DIST = $(TESTS) \
	     $(top_srcdir)/tests/testfiles/*.owfn \
	     $(top_srcdir)/tests/testfiles/*.owfn.expected