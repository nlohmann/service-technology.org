
############################################################################
# All tests that are expected to succeed (the default case) 
############################################################################
TESTS = cmdline_test.sh parser_run.sh parser_filter_run.sh compiler_allformats.sh compiler_soundness.sh

############################################################################
# All tests that are expected to fail (must be a subset of TESTS above)
# e.g., XFAIL_TESTS = compiler_allformats.sh 
############################################################################
XFAIL_TESTS = compiler_allformats.sh


# Notice: If you add environment variables here, you have to provide defaults
# defaults.sh, too.
TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = *.lola \
		*.task \
		*.xml \
		*.dot \
		*.owfn \
		*.png \
		uml2owfn.exe.stackdump \
		core \
		result.txt \
		*.sh \
		*.csv

EXTRA_DIST = $(TESTS) compiler soundness
