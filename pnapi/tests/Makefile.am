#############################################################################
# You need to perform three steps to add a test case:
#   1.) edit the file `testsuite.at' and add your test case
#   2.) add the files you need for the test to the `testfile' directory
#   3.) add these files (without path) to the `TESTFILES' list
# Do not forget to add your test files to the repository!
#############################################################################

# the files needed by the test cases (without `testfiles/' directory)
TESTFILES = test_input.owfn syntax_example1.owfn syntax_example2.owfn \
            syntax_example3.owfn syntax_example4.owfn bug13579.owfn \
            bug13968.sa bug13990.1.owfn bug13990.2.owfn bug13990.3.owfn \
            bug14064.owfn bug14116.owfn bug14117.sa bug14119.1.owfn \
            bug14119.2.owfn bug14125.1.owfn bug14125.2.owfn bug14127.owfn \
            bug14128.1.owfn bug14128.2.owfn bug14206.lola bug14208.1.owfn \
            bug14208.2.owfn bug14335.owfn bug14411.owfn \
            bug14411-constraint.owfn bug14417.1.owfn bug14417.2.owfn \
            bug14856.owfn bug15415.owfn data.hlowfn eisen.hlowfn myCoffee.owfn phils.hlowfn \
            Model.1bt1--0-----u--.xml.tpn Model.7c1w--0-----u--.xml.tpn \
	    Model.4ose--0-----u--.xml.tpn Model.3wae--0-----u--.xml.tpn \
	    Model.5e1y--0-----u--.xml.tpn Model.6mxu--0-----u--.xml.tpn \
            test_cdnf.owfn test_input.lola test_dead_nodes.net.owfn \
            test_dnf.owfn test_identical_places.net.owfn \
            test_identical_transitions.net.owfn test_input_roles.owfn \
            test_input_roles_error.owfn test_self_loop_places.net.owfn \
            test_self_loop_transitions.net.owfn test_series_places.net.owfn \
            test_series_transitions.net.owfn \
            test_series_transitions_keepnormal.net.owfn \
            test_starke_3p.net.owfn test_starke_3t.net.owfn \
            test_starke_4.net.owfn test_starke_5.net.owfn \
            test_starke_6.net.owfn test_starke_7.net.owfn \
            test_starke_8.net.owfn test_starke_9.net.owfn \
            test_criteria.noworkflownet.owfn test_criteria.workflownet.owfn \
            test_criteria.nofreechoice.owfn test_criteria.freechoice.owfn \
            test_construction.sequence_normal.net.owfn \
            test_construction.sequence_unnormal.net.owfn \
            test_construction.strange_net.net.owfn test_myCoffee.sa \
            test_myCoffee-sm.owfn test_myCoffee-syn.owfn testnet.pnml \
            syntax_example.owfn syntax_example_new.owfn syntax_example_new_short.owfn \
            syntax_formula.owfn syntax_max_occurrence.owfn \
            test_produce.onlineshop.constraint.owfn \
            test_produce.onlineshop.owfn test_automaton_behavior_async.owfn \
            fiona_abnormal_both.normalized.expected.owfn \
            fiona_abnormal_both.owfn \
            fiona_abnormal_both2.normalized.expected.owfn \
            fiona_abnormal_both2.owfn \
            fiona_abnormal_input.normalized.expected.owfn \
            fiona_abnormal_input.owfn \
            fiona_abnormal_output.normalized.expected.owfn \
            fiona_abnormal_output.owfn fiona_coffee.normalized.expected.owfn \
            fiona_coffee.owfn zeus.owfn test_input.pnml cyclicFullCoffee.pnml \
            cyclicFullCoffee.owfn phils.pnml phils.lola \
            prom-2_7.pnml prom-Heuristics.pnml \
            prom-OverlyPreciseModel_artificialStartEndTasks.pnml \
            prom-peller.pnml prom-ADtranslationAccordingToSpec.pnml \
            prom-M1_nonFitting.pnml prom-PetriNet.pnml \
            prom-pellerMaarDanMetReset.pnml prom-Alpha.pnml \
            prom-M2_underfitting.pnml prom-petrify_learn01_5cv.pnml \
            prom-PetriNet_split_onderzoek_behandeling.pnml \
            prom-AlphaPlusPlus.pnml prom-M3_overfitting.pnml \
            prom-Petrinet_diagnosis.pnml prom-petrify_learn02_5cv.pnml \
            prom-BadStructure.pnml prom-M4_perfectNet.pnml prom-Petriy.pnml \
            prom-petrify_learn03_5cv.pnml prom-DuplicatesGenetic.pnml \
            prom-M5_structuralInappropriate.pnml prom-ReferenceModel.pnml \
            prom-petrify_learn04_5cv.pnml prom-GenDupl.pnml \
            prom-M6_behaviouralInappropriate.pnml prom-aab.pnml \
            prom-petrify_learn05_5cv.pnml prom-GenDuplicates.pnml \
            prom-NonFitting.pnml prom-componentnet1.pnml prom-phils.pnml \
            prom-Genetic.pnml prom-NonFittingModel.pnml prom-componentnet2.pnml \
            prom-runningExample.pnml prom-Genetic_withLazyTasks.pnml \
            prom-NonFittingModel_artificialStartEndTasks.pnml prom-eric.pnml \
            prom-runningExample_Alpha.pnml prom-GoodModel.pnml \
            prom-Onderzoek_voor_Behandeling.pnml prom-erm_model.pnml \
            prom-runningExample_AlphaPlusPlus.pnml \
            prom-GoodModel_artificialStartEndTasks.pnml prom-OverlyGeneral.pnml \
            prom-freechoicetelim.pnml prom-runningExample_petriy.pnml \
            prom-Heuristic.pnml prom-OverlyGeneralModel.pnml \
            prom-g24_alphaAlgorithm.pnml prom-test1.pnml \
            prom-HeuristicMiner.pnml \
            prom-OverlyGeneralModel_artificialStartEndTasks.pnml \
            prom-g24_alphaPlusPlusAlgorithm.pnml prom-testnet.pnml \
            prom-Heuristic_withLazyTasks.pnml prom-OverlyPreciseModel.pnml \
            prom-originalModel.pnml N0.owfn N1.owfn N2.owfn N3.owfn \
            travel.owfn travel2.owfn prom-bla.pnml


# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sourses called 'cmdline.c'.
NO_COVER_FILES = */cmdline.c */parser-*.cc



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

################
# GNU autotest #
################

TESTSUITE = $(builddir)/testsuite
EXTRA_DIST = testsuite.at wrap.sh \
             $(addprefix testfiles/,$(TESTFILES))
DISTCLEANFILES = atconfig package.m4 $(TESTSUITE)

# run the test suite when calling "make check"
check-local: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

# clean the test results on "make clean"
clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

# call a specific test set, (e.g. "make check-basic")
check-%: atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) --keywords=$*

# create the testsuite using autom4te
$(TESTSUITE): $(srcdir)/testsuite.at
	$(AUTOM4TE) --language=autotest -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@


######################
# LCOV test coverage #
######################

# distribute the LGOV scripts
EXTRA_DIST += lcov/README lcov/gendesc lcov/genhtml lcov/geninfo lcov/genpng lcov/lcov

# compile the code for code coverage
cover-prepare:
	$(MAKE) --directory=$(top_srcdir) clean all \
	  CXXFLAGS+="-fprofile-arcs -ftest-coverage" \
	  CFLAGS+="-fprofile-arcs -ftest-coverage" \
	  LDFLAGS+="-lgcov"

# get descriptions from the testsuite
descriptions: $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' -l > descriptions.txt
	$(SED) -i '1,5d;n;d;' descriptions.txt
	$(SED) -i 's/:[ \t]*testsuite.at:[0-9]*[ \t]*/ /;s/^[ \t]*//' descriptions.txt
	$(SED) -i 's/ /\n\t/' descriptions.txt
	lcov/gendesc descriptions.txt -o descriptions1
	cp descriptions1 descriptions10
	cp descriptions1 descriptions100
	$(SED) -i 's/^TN: \([1-9]\)$$/TN: 0\1/' descriptions10
	$(SED) -i 's/^TN: \([1-9]\)$$/TN: 00\1/' descriptions100
	$(SED) -i 's/^TN: \([1-9][1-9]\)$$/TN: 0\1/' descriptions100
	cat descriptions1 descriptions10 descriptions100 > descriptions
	rm descriptions1 descriptions10 descriptions100 descriptions.txt

# execute tests using coverage
cover-check: cover-prepare $(TESTSUITE)
	test ! -d cover || rm -fr cover
	$(MKDIR_P) cover
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) COVER=TRUE

# exclude generated code using the wildcards specified in NO_COVER_FILES
cover.info:
	cat cover/*.info > cover.info
	lcov/lcov --remove cover.info $(addprefix ",$(addsuffix ", $(NO_COVER_FILES))) -o cover.info

# create a HTML table with results from LCOV
cover-html: descriptions cover.info
	lcov/genhtml cover.info \
	   --output-directory cover-html --title "$(PACKAGE_STRING)" \
	   --show-details --legend --description-file descriptions --sort \
	   --no-function-coverage --prefix $(abs_top_builddir)
	@echo see `pwd`/cover-html/index.html for a coverage report

cover: cover-check cover-html

# use ZCOV for branch coverage
cover-branch: cover-prepare $(TESTSUITE)
	test ! -d cover || rm -fr cover
	$(MKDIR_P) cover
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) COVER=ZCOV
	zcov/zcov-merge cover/output.zcov cover/*.zcov
	zcov/zcov-genhtml cover/output.zcov cover-html
	@echo see `pwd`/cover-html/index.html for a coverage report

# clean also the temp files in the src directory
CLEANFILES = -fr cover descriptions 
MAINTAINERCLEANFILES = -fr cover-html cover.info
maintainer-clean-local:
	find $(top_builddir) -name *.gcov -exec rm {} \;
	find $(top_builddir) -name *.gcno -exec rm {} \;
	find $(top_builddir) -name *.gcda -exec rm {} \;

