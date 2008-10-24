include $(top_srcdir)/Makefile.covcommon


############################################################################
# All tests that are expected to succeed (the default case) 
############################################################################
TESTS = slist.sh syntax.sh extrema.sh cyclic.sh messagebound.sh png.sh \
	countservices.sh falseannos.sh oWFNreduction.sh sim_og.sh \
	matching.sh productog.sh publicview.sh samples.sh partner.sh \
	minimize.sh bddtest.sh statesReduction.sh responsive.sh normalize.sh


############################################################################
# All tests that are expected to fail (must be a subset of TESTS above)
# e.g., XFAIL_TESTS = bddtest.sh 
############################################################################
#XFAIL_TESTS = bddtest.sh 


check_PROGRAMS = slistcheck
slistcheck_SOURCES = slistcheck.cc
slistcheck_CXXFLAGS = -I$(top_srcdir)/src @configured_CXXFLAGS@
slistcheck_LDFLAGS = @configured_LDFLAGS@

# Notice: If you add environment variables here, you have to provide defaults
# defaults.sh, too.
TESTS_ENVIRONMENT = testdir=$(srcdir) MKDIR_P="$(MKDIR_P)" \
	builddir=$(builddir) PATH=".:../src:$(srcdir):$$PATH"

MEMCHECK_ENVIRONMENT = $(TESTS_ENVIRONMENT) memcheck=yes
CHECKLOUD_ENVIRONMENT = $(TESTS_ENVIRONMENT) quiet=no

# For maximum portability we mention every file explicitly:
#   o $(wildcard *.owfn) is not portable, but requires GNU make.
#   o *.owfn fails for VPATH builds.
#   o Messing around with VPATH is ugly.
#   o More reasons at http://www.gnu.org/software/automake/manual/html_mono/automake.html#wildcards
EXTRA_DIST = $(TESTS) memcheck_helper.sh \
	defaults.sh \
	evaluateExitCode.sh \
	syntax/max_occurrence.owfn \
	syntax/syntax_example.owfn \
	syntax/syntax_example_new.owfn \
	syntax/syntax_example_new_short.owfn \
	syntax/formula.owfn \
	syntax/formula.owfn.expected.og \
	extrema/all_final_marking.owfn \
	extrema/empty.owfn \
	extrema/initial_marking_midways.owfn \
	extrema/initialmarking_is_a_finalmarking.owfn \
	extrema/innerUnbounded.owfn \
	extrema/multiple_input.owfn \
	extrema/multiple_input_multiple_output.owfn \
	extrema/multiple_output.owfn \
	extrema/no_communication.owfn \
	extrema/no_final_marking.owfn \
	extrema/no_initial_marking.owfn \
	extrema/no_initial_marking2.owfn \
	extrema/one_input.owfn \
	extrema/one_input_2.owfn \
	extrema/one_input_marked.owfn \
	extrema/one_input_one_output.owfn \
	extrema/one_output.owfn \
	extrema/one_output_marked.owfn \
	extrema/transient_finalmarking.owfn \
	extrema/non_empty_true_node.owfn \
	extrema/non_empty_true_node2.owfn \
	cyclic/livelockInP.owfn \
	cyclic/zyklusP.owfn \
	cyclic/zyklusPmitCommit.owfn \
	cyclic/zyklusR.owfn \
	cyclic/zyklusReturn.owfn \
	cyclic/livelockInP.owfn.png \
	cyclic/zyklusP.owfn.png \
	cyclic/zyklusPmitCommit.owfn.png \
	cyclic/zyklusR.owfn.png \
	cyclic/zyklusReturn.owfn.png \
	messagebound/innerBounded.owfn \
	messagebound/messageboundtest1.owfn \
	messagebound/messageboundtest2.owfn \
	messagebound/messageboundtest3.owfn \
	messagebound/messageboundtest4.owfn \
	messagebound/messageboundtest5.owfn \
	messagebound/unlimited-communication.owfn \
	png/big.owfn \
	png/06-03-23_BPM06_shop_sect_3.owfn.og \
	countservices/acyclic.og \
	countservices/cyclic.og \
	countservices/nostrat.og \
	countservices/false_nodes.og \
	countservices/multiple_arcs.og \
	countservices/sequence4.owfn.og \
	falseannos/bluefalsenode.expected.og \
	falseannos/bluefalsenode.og \
	falseannos/shorten.expected.og \
	falseannos/shorten.og \
	falseannos/violating.expected.og \
	falseannos/violating.og \
	oWFNreduction/060116-misc-coffee.owfn \
	oWFNreduction/phcontrol3.unf.owfn \
	oWFNreduction/06-03-23_BPM06_shop_sect_6.owfn \
	sim_og/coarse-chopped.og \
	sim_og/coarse.og \
	sim_og/compact.og \
	sim_og/cyclic1.og \
	sim_og/cyclic2.og \
	sim_og/trafo_rule3_N7.owfn \
	sim_og/trafo_rule3_N8.owfn \
	sim_og/trafo_rule4_N9.owfn \
	sim_og/trafo_rule4_N10.owfn \
	sim_og/trafo_wsfm_M0.owfn \
	sim_og/trafo_wsfm_M1.owfn \
	sim_og/trafo_wsfm_M2.owfn \
	sim_og/trafo_wsfm_M3.owfn \
	matching/airline.og \
	matching/airline.owfn \
	matching/agency_traveler.og \
	matching/agency_traveler.owfn \
	matching/client_match_1.owfn \
	matching/client_nomatch_1.owfn \
	matching/client_nosupport_1.owfn \
	matching/shop.og \
	matching/shop_rednodes.og \
	matching/interface_rcv-a_snd-b.og \
	matching/interface_rcv-a_snd-b.og.png \
	matching/interface_rcv-a_snd-by.owfn \
	matching/interface_rcv-a_snd-by.owfn.png \
	matching/interface_rcv-ax_snd-b.owfn \
	matching/interface_rcv-ax_snd-b.owfn.png \
	normalize/abnormal_input.owfn \
	normalize/abnormal_input.normalized.expected.owfn \
	normalize/abnormal_output.owfn \
	normalize/abnormal_output.normalized.expected.owfn \
	normalize/abnormal_both.owfn \
	normalize/abnormal_both.normalized.expected.owfn \
	normalize/abnormal_both2.owfn \
	normalize/abnormal_both2.normalized.expected.owfn \
	normalize/coffee.owfn \
	normalize/coffee.normalized.expected.owfn \
	productog/1.og \
	productog/2.og \
	productog/3.og \
	productog/1x2.expected.og \
	productog/1x2x3.expected.og \
	productog/3x1x2.expected.og \
	publicview/complexPV.owfn \
	publicview/non_empty_true_node2.owfn \
	publicview/non_empty_true_node3.owfn \
	publicview/NonFinalTauNode1.owfn \
	publicview/NonFinalTauNode2.owfn \
	publicview/NonFinalTauNode3.owfn \
	publicview/NonFinalTauNode4.owfn \
	publicview/NonFinalTauNode1not.owfn \
	publicview/NonFinalTauNode2not.owfn \
	samples/sequence9.owfn \
	samples/06-03-23_BPM06_shop_sect_3.owfn \
	samples/06-03-23_BPM06_shop_sect_6.owfn \
	samples/dke07_shop_sect_3.owfn \
	samples/dke07_shop_sect_6.owfn \
	samples/keesCoffee_condition.owfn \
	samples/myCoffee.owfn \
	partner/sequence3-partner.expected.owfn \
	partner/coffee.owfn \
	partner/coffee-partner.expected.owfn \
	partner/coffee2.owfn \
	partner/coffee2-partner.expected.owfn \
	partner/coffee_extended.owfn \
	partner/coffee_extended-partner.expected.owfn \
	partner/sequence3.owfn \
	minimize/nostrat.og \
	minimize/final_non-final.og \
	minimize/non-final_final.og \
	minimize/4-loop.og \
	minimize/sample.og \
	minimize/myCoffee.og \
	minimize/keesCoffee_condition.og \
	minimize/keesCoffee_condition.emptyNode.og \
	minimize/06-03-23_BPM06_shop_sect_3.owfn.og \
	statesReduction/06-03-23_BPM06_shop_sect_3.owfn \
	statesReduction/06-03-23_BPM06_shop_sect_6.owfn \
	statesReduction/dke07_shop_sect_3.owfn \
	statesReduction/dke07_shop_sect_6.owfn \
	statesReduction/myCoffee.owfn \
	statesReduction/Reservations.owfn \
	bdd_ref/phcontrol4.unf_reference.owfn \
	bdd_ref/sequence3_reference.owfn \
	bdd_ref/myCoffee_reference.owfn \
	bdd_ref/myCoffee_reference.owfn.og.BDD_ANN.cudd \
	bdd_ref/myCoffee_reference.owfn.og.BDD_MP.cudd \
	bdd_ref/myCoffee_reference.owfn.og.png \
	bdd_ref/phcontrol4.unf_reference.owfn.og.BDD_ANN.cudd \
	bdd_ref/phcontrol4.unf_reference.owfn.og.BDD_MP.cudd \
	bdd_ref/sequence3_reference.owfn.og.BDD_ANN.cudd \
	bdd_ref/sequence3_reference.owfn.og.BDD_MP.cudd \
	bdd_ref/phcontrol4.unf_reference.owfn.og.png \
	bdd_ref/sequence3_reference.owfn.og.png \
	bdd_ref/myCoffee.owfn \
	bdd_ref/myCoffee_reference.owfn \
	bdd_ref/myCoffee_reference.owfn.og.BDD_ANN.cudd \
	bdd_ref/myCoffee_reference.owfn.og.BDD_MP.cudd \
	bdd_ref/myCoffee_reference_assignments.txt \
	sequence_suite/sequence10.owfn \
	sequence_suite/sequence11.owfn \
	sequence_suite/sequence12.owfn \
	sequence_suite/sequence13.owfn \
	sequence_suite/sequence14.owfn \
	sequence_suite/sequence15.owfn \
	sequence_suite/sequence16.owfn \
	sequence_suite/sequence17.owfn \
	sequence_suite/sequence18.owfn \
	sequence_suite/sequence19.owfn \
	sequence_suite/sequence20.owfn \
	sequence_suite/sequence3.owfn \
	sequence_suite/sequence4.owfn \
	sequence_suite/sequence5.owfn \
	sequence_suite/sequence6.owfn \
	sequence_suite/sequence7.owfn \
	sequence_suite/sequence8.owfn \
	sequence_suite/sequence9.owfn \
	philosophers/phcontrol10.unf.owfn \
	philosophers/phcontrol3.unf.owfn \
	philosophers/phcontrol4.unf.owfn \
	philosophers/phcontrol5.unf.owfn \
	philosophers/phcontrol6.unf.owfn \
	philosophers/phcontrol7.unf.owfn \
	philosophers/phcontrol8.unf.owfn \
	philosophers/phcontrol9.unf.owfn \
	philosophers/phcontrol10.unf.owfn.png \
	philosophers/phcontrol3.unf.owfn.png \
	philosophers/phcontrol4.unf.owfn.png \
	philosophers/phcontrol5.unf.owfn.png \
	philosophers/phcontrol6.unf.owfn.png \
	philosophers/phcontrol7.unf.owfn.png \
	philosophers/phcontrol8.unf.owfn.png \
	philosophers/phcontrol9.unf.owfn.png \
	responsive/controllable_not_responsive_controllable.owfn \
	responsive/controllable_and_responsive_controllable.owfn \
	responsive/finalmarking_in_TSCC.owfn \
	responsive/2_TSCCs_in_1_node_with_communication_1.owfn \
	responsive/2_TSCCs_in_1_node_with_communication_2.owfn \
	responsive/cgvExample.owfn
	

# Global check-loud target. Runs all tests. Output files are generated.
check-loud:
	$(MAKE) $(AM_MAKEFLAGS) $(CHECKLOUD_ENVIRONMENT) check


# Generate check targets from $(TESTS). That is done by removing '.sh' from
# every word in $(TESTS) and precede them by 'check-'. So one of many check
# targets would be "check-samples" if "samples.sh" was in $(TESTS).
SINGLE_CHECK_TARGETS = $(TESTS:%.sh=check-%)

# The same for loud (unquiet, output files generating)
SINGLE_CHECKLOUD_TARGETS = $(TESTS:%.sh=check-%-loud)

# All single check targets. For example check-samples.
# The script belonging to the current check target is inferred from the
# name of the target by removing the initial 'check-' and adding '.sh' to
# the end.
$(SINGLE_CHECK_TARGETS):
	-$(TESTS_ENVIRONMENT) ./$(@:check-%=%.sh)

$(SINGLE_CHECKLOUD_TARGETS):
	-$(CHECKLOUD_ENVIRONMENT) ./$(@:check-%-loud=%.sh)

# Global memcheck target. Runs all tests under valgrind.
memcheck:
	$(MAKE) $(AM_MAKEFLAGS) $(MEMCHECK_ENVIRONMENT) check

# Create single memcheck target analogous to single check targets
SINGLE_MEMCHECK_TARGETS = $(TESTS:%.sh=memcheck-%)
$(SINGLE_MEMCHECK_TARGETS):
	-$(MEMCHECK_ENVIRONMENT) ./$(@:memcheck-%=%.sh)

COV_RUNFILES=cov-run-*.covexp
COV_DBFILE=cov-db
COV_ALLFILES=$(COV_RUNFILES) $(COV_DBFILE)

# Coverage check
covcheck: covrunclean
	$(MAKE) $(AM_MAKEFLAGS) CXX="$(CXXCOV)" $(check_PROGRAMS)
	-$(MAKE) $(AM_MAKEFLAGS) cleandeps
	-$(MAKE) $(AM_MAKEFLAGS) check
	covmerge.exe $(COV_RUNFILES) > $(COV_DBFILE)
	gen_html $(COV_DBFILE)
	$(MAKE) $(AM_MAKEFLAGS) covrunclean

covrunclean:
	-test -z "$(COV_ALLFILES)" || rm -f $(COV_ALLFILES)

#[NL] clean-local: covrunclean cleandeps
clean-local: covrunclean

DISTCLEANFILES =  productog/1x2.output.og \
	productog/1x2.output.out \
	productog/1x2.output.png \
	productog/1x2x3.output.og \
	productog/1x2x3.output.out \
	productog/1x2x3.output.png \
	productog/3x1x2.output.og \
	productog/3x1x2.output.out \
	productog/3x1x2.output.png \
	formula/*output* \
	petrify.log \
	falseannos/bluefalsenode.blue.og \
	falseannos/shorten.blue.og \
	falseannos/violating.blue.og \
	statesReduction/06-03-23_BPM06_shop_sect_3.owfn.output.ig.og \
	statesReduction/06-03-23_BPM06_shop_sect_3.owfn.output.R.ig.og \
	statesReduction/06-03-23_BPM06_shop_sect_3.owfn.output.og \
	statesReduction/06-03-23_BPM06_shop_sect_3.owfn.output.R.og \
	statesReduction/06-03-23_BPM06_shop_sect_6.owfn.output.ig.og \
	statesReduction/06-03-23_BPM06_shop_sect_6.owfn.output.R.ig.og \
	statesReduction/myCoffee.owfn.output.ig.og \
	statesReduction/myCoffee.owfn.output.R.ig.og \
	statesReduction/dke07_shop_sect_3.owfn.output.ig.og \
	statesReduction/dke07_shop_sect_3.owfn.output.R.ig.og \
	statesReduction/dke07_shop_sect_6.owfn.output.ig.og \
	statesReduction/dke07_shop_sect_6.owfn.output.R.ig.og \
	minimize/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.og \
	minimize/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.out \
	minimize/06-03-23_BPM06_shop_sect_3.owfn.og.minimal.png \
	minimize/4-loop.og.minimal.og \
	minimize/4-loop.og.minimal.out \
	minimize/4-loop.og.minimal.png \
	minimize/final_non-final.og.minimal.og \
	minimize/final_non-final.og.minimal.out \
	minimize/final_non-final.og.minimal.png \
	minimize/keesCoffee_condition.emptyNode.og.minimal.og \
	minimize/keesCoffee_condition.emptyNode.og.minimal.out \
	minimize/keesCoffee_condition.emptyNode.og.minimal.png \
	minimize/keesCoffee_condition.og.minimal.og \
	minimize/keesCoffee_condition.og.minimal.out \
	minimize/keesCoffee_condition.og.minimal.png \
	minimize/myCoffee.og.minimal.og \
	minimize/myCoffee.og.minimal.out \
	minimize/myCoffee.og.minimal.png \
	minimize/non-final_final.og.minimal.og \
	minimize/non-final_final.og.minimal.out \
	minimize/non-final_final.og.minimal.png \
	minimize/nostrat.og.minimal.og \
	minimize/nostrat.og.minimal.out \
	minimize/nostrat.og.minimal.png \
	minimize/sample.og.minimal.og \
	minimize/sample.og.minimal.out \
	minimize/sample.og.minimal.png \
	oWFNreduction/06-03-23_BPM06_shop_sect_6.reduced.owfn \
	oWFNreduction/060116-misc-coffee.reduced.owfn \
	oWFNreduction/phcontrol3.unf.reduced.owfn \
	partner/sequence3.owfn.og \
	partner/sequence3-partner.owfn \
	partner/sequence3-partner.dot \
	partner/sequence3-partner.png \
	partner/sequence3.owfn.og.og.pn \
	partner/sequence3.owfn.og.og.stg \
	partner/sequence3.owfn.og.out \
	partner/sequence3.owfn.og.png \
	partner/sequence3.owfn.og.stg \
	partner/sequence3.owfn.og.pn \
	partner/coffee-partner.dot \
	partner/coffee-partner.owfn \
	partner/coffee-partner.png \
	partner/coffee.owfn.ig.og \
	partner/coffee.owfn.ig.out \
	partner/coffee.owfn.ig.pn \
	partner/coffee.owfn.ig.png \
	partner/coffee.owfn.ig.stg \
	partner/coffee2-partner.dot \
	partner/coffee2-partner.owfn \
	partner/coffee2-partner.png \
	partner/coffee2.owfn.ig.og \
	partner/coffee2.owfn.ig.out \
	partner/coffee2.owfn.ig.pn \
	partner/coffee2.owfn.ig.png \
	partner/coffee2.owfn.ig.stg \
	partner/coffee_extended-partner.dot \
	partner/coffee_extended-partner.owfn \
	partner/coffee_extended-partner.png \
	partner/coffee_extended.owfn.ig.og \
	partner/coffee_extended.owfn.ig.out \
	partner/coffee_extended.owfn.ig.pn \
	partner/coffee_extended.owfn.ig.png \
	partner/coffee_extended.owfn.ig.stg \
	png/big.owfn.output.dot \
	png/big.owfn.output.out \
	png/big.owfn.output.png \
	png/big.owfn.output.tex \
	png/big.owfn.output.og \
	png/big.owfn.output.og.dot \
	png/big.owfn.output.og.out \
	png/big.owfn.output.og.png \
	png/big.owfn.output.og.tex \
	png/06-03-23_BPM06_shop_sect_3.owfn.og.output.dot \
	png/06-03-23_BPM06_shop_sect_3.owfn.og.output.out \
 	png/06-03-23_BPM06_shop_sect_3.owfn.og.output.png \
	png/06-03-23_BPM06_shop_sect_3.owfn.og.output.tex \
	publicview/complexPV.pv.owfn \
	publicview/complexPV.pv.sa.out \
	publicview/complexPV.pv.sa.png \
	publicview/non_empty_true_node2.pv.owfn \
	publicview/non_empty_true_node2.pv.sa.out \
	publicview/non_empty_true_node2.pv.sa.png \
	publicview/non_empty_true_node3.pv.owfn \
	publicview/non_empty_true_node3.pv.sa.out \
	publicview/non_empty_true_node3.pv.sa.png \
	publicview/NonFinalTauNode1.pv.sa.out \
	publicview/NonFinalTauNode1.pv.owfn \
	publicview/NonFinalTauNode2.pv.sa.out \
	publicview/NonFinalTauNode2.pv.owfn \
	publicview/NonFinalTauNode3.pv.sa.out \
	publicview/NonFinalTauNode3.pv.owfn \
	publicview/NonFinalTauNode4.pv.sa.out \
	publicview/NonFinalTauNode4.pv.owfn \
	publicview/NonFinalTauNode1not.pv.sa.out \
	publicview/NonFinalTauNode1not.pv.owfn \
	publicview/NonFinalTauNode2not.pv.sa.out \
	publicview/NonFinalTauNode2not.pv.owfn \
	syntax/formula.owfn.output.og \
	syntax/formula.owfn.output.og.out \
	philosophers/phcontrol4.unf.owfn.og.png \
	philosophers/phcontrol4.unf.owfn.og.out \
	philosophers/phcontrol4.unf.owfn.og.BDD_MP.cudd \
	philosophers/phcontrol4.unf.owfn.og.BDD_ANN.cudd \
	bdd_ref/myCoffee.owfn.og.out \
	bdd_ref/myCoffee.owfn.og \
	bdd_ref/myCoffee.owfn.og.BDD_ANN.cudd \
	bdd_ref/myCoffee.owfn.og.BDD_MP.cudd \
	bdd_ref/myCoffee.owfn.og.png	


distclean-local:
	-test -d coverage_html/ && rm -rf coverage_html/
	rm -rf $(DISTCLEAN_FILES)    
