TESTS = test1 test2 test3

BPEL_PROCESSES = error.bpel loan.bpel purchase.bpel shipping.bpel

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = $(BPEL_PROCESSES:.bpel=.info) \
	     $(BPEL_PROCESSES:.bpel=.lola) \
	     $(BPEL_PROCESSES:.bpel=.owfn) \
	     $(BPEL_PROCESSES:.bpel=.dot) \
	     $(BPEL_PROCESSES:.bpel=.xml) \
	     $(BPEL_PROCESSES:.bpel=.ast)
	     
EXTRA_DIST = $(TESTS) $(BPEL_PROCESSES)
