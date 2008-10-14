TESTS = test-cmdline \
	test-cmdline-help \
	test-cmdline-version \
	test-cmdline-bug \
	test-cmdline-configfile \
	test-cmdline-configfile-missing \
	test-cmdline-full-help \
	test-cmdline-missing \
	test-simulation \
	test-simulation-dot \
	test-matching \
	test-matching-verbose \
	test-frontend-full \
	test-frontend-error-lexer \
	test-frontend-error-parser \
	test-noop

XFAIL_TESTS = test-cmdline \
	test-cmdline-configfile-missing \
	test-cmdline-missing \
	test-frontend-error-lexer \
	test-frontend-error-parser

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

EXTRA_DIST = $(TESTS) testfiles \
	lcov/README lcov/gendesc lcov/genhtml \
	lcov/geninfo lcov/genpng lcov/lcov

CLEANFILES = -fr output $(COVERINFOS) \
	traveler.sa_agency_airline.og_simulation.dot*


##############################################################################
# all you need for code coverage
##############################################################################

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
cover: $(COVERINFOS)
	$(GENHTML) $(COVERINFOS) \
	   --output-directory output --title "$(PACKAGE_STRING)" \
	   --show-details --legend
