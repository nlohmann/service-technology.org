TESTS = test-cmdline-help\
 		test-cmdline-full-help\
 		test-cmdline-detailed-help\
		test-cmdline-bug\
		test-cmdline-version\
		test-input-file-error\
		test-input-pipe\
		test-input-pipe-error\
		test-input-file-auction\
		test-input-file-loan\
		test-input-file-order\
		test-input-file-purchase\
		test-input-file-shipping\
		test-input-file-loan11\
		test-input-file-purchase11\
		test-input-file-shipping11\
		test-output-apnn\
		test-output-ina\
		test-output-lola\
		test-output-owfn\
		test-output-pep\
		test-output-pnml\
		test-output-spin\
		test-output-tpn\
		test-output-dot\
		test-output-xml\
		test-choreography-customer1\
		test-choreography-customer2\
		test-choreography-customer3\
		test-bpel4chor-1instance\
		test-bpel4chor-10instances\
		test-staticanalysis-sa00002\
		test-staticanalysis-sa00003\
		test-staticanalysis-sa00005\
		test-staticanalysis-sa00006\
		test-staticanalysis-sa00007\
		test-staticanalysis-sa00008\
		test-staticanalysis-sa00015\
		test-staticanalysis-sa00016\
		test-staticanalysis-sa00017\
		test-staticanalysis-sa00018\
		test-staticanalysis-sa00023\
		test-staticanalysis-sa00024\
		test-staticanalysis-sa00025\
		test-staticanalysis-sa00032\
		test-staticanalysis-sa00035\
		test-staticanalysis-sa00036\
		test-staticanalysis-sa00037\
		test-staticanalysis-sa00044\
		test-staticanalysis-sa00048\
		test-staticanalysis-sa00051\
		test-staticanalysis-sa00052\
		test-staticanalysis-sa00055\
		test-staticanalysis-sa00056\
		test-staticanalysis-sa00057\
		test-staticanalysis-sa00058\
		test-staticanalysis-sa00059\
		test-staticanalysis-sa00060\
		test-staticanalysis-sa00061\
		test-staticanalysis-sa00062\
		test-staticanalysis-sa00063\
		test-staticanalysis-sa00064\
		test-staticanalysis-sa00065\
		test-staticanalysis-sa00066\
		test-staticanalysis-sa00067\
		test-staticanalysis-sa00068\
		test-staticanalysis-sa00069\
		test-staticanalysis-sa00070\
		test-staticanalysis-sa00071\
		test-staticanalysis-sa00072\
		test-staticanalysis-sa00073\
		test-staticanalysis-sa00074\
		test-staticanalysis-sa00075\
		test-staticanalysis-sa00076\
		test-staticanalysis-sa00077\
		test-staticanalysis-sa00078\
		test-staticanalysis-sa00079\
		test-staticanalysis-sa00080\
		test-staticanalysis-sa00081\
		test-staticanalysis-sa00082\
		test-staticanalysis-sa00083\
		test-staticanalysis-sa00084\
		test-staticanalysis-sa00087\
		test-staticanalysis-sa00088\
		test-staticanalysis-sa00091\
		test-staticanalysis-sa00092\
		test-staticanalysis-sa00093

XFAIL_TESTS = test-input-file-error test-input-pipe-error\
			  test-staticanalysis-sa00007 test-staticanalysis-sa00065 test-staticanalysis-sa00077 test-staticanalysis-sa00078

TESTS_ENVIRONMENT = testdir=$(top_srcdir)/tests PATH=.:../src:$$PATH

CLEANFILES = -fr output $(COVERINFOS) \
             *.task \
	     	 *.xml \
	     	 *.dot \
	     	 *.apnn *.pnt *.lola *.owfn *.ll_net *.pnml *.spin *.tpn \
	     	 *.png \
	     	 bpel2owfn.exe.stackdump \
	     	 core
	     
EXTRA_DIST = $(TESTS) \
	     testfiles \
	     sa_tests \
	     consistency \
	     bpel4chor


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
