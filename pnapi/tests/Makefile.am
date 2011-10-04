#============================================================================#
# AUTOTEST MAKEFILE                                                          #
# skeleton written by Niels Lohmann <niels.lohmann@uni-rostock.de>           #
#============================================================================#

# This Makefile organizes the generation of the test suite, the code coverage
# and the packaging of the tests. It consists of three parts, of which you
# need to change the first if you want to add test files and the second if
# you want to exclude files from the test case coverage (very seldomly). The
# last part is generic and you must not change it.


#--------------------#
# Part 1: Test Files #
#--------------------#

# If you want to use a new test file in a test case, proceed as follows:
# 1. Edit the file `testsuite.at' and add your test case
# 2. Add the files you need for the test to the `testfile' directory.
# 3. Add these files (without path) to the `TESTFILES' list.
# 4. Add the file to the SVN repository.
TESTFILES = test_input.owfn syntax_example1.owfn syntax_example2.owfn \
            syntax_example3.owfn syntax_example4.owfn bug13579.owfn \
            bug13968.sa bug13990.1.owfn bug13990.2.owfn bug13990.3.owfn \
            bug14064.owfn bug14116.owfn bug14117.sa bug14119.1.owfn \
            bug14119.2.owfn bug14127.owfn \
            bug14128.1.owfn bug14128.2.owfn bug14206.lola bug14208.1.owfn \
            bug14208.2.owfn bug14335.owfn bug14411.owfn \
            bug14411-constraint.owfn bug14417.1.owfn bug14417.2.owfn \
            bug14856.owfn bug15415.owfn bug16302.owfn \
            bug_15820_sp.owfn bug_15820_sp_2.owfn \
            data.hlowfn eisen.hlowfn myCoffee.owfn phils.hlowfn \
            error_parser_lola_01.lola error_parser_lola_02.lola \
            error_parser_lola_03.lola \
            error_parser_owfn_01.owfn error_parser_owfn_02.owfn \
            error_parser_owfn_03.owfn error_parser_owfn_04.owfn \
            error_parser_owfn_05.owfn error_parser_owfn_06.owfn \
            error_parser_owfn_07.owfn error_parser_owfn_08.owfn \
            error_parser_owfn_09.owfn error_parser_owfn_10.owfn \
            error_parser_owfn_11.owfn error_parser_owfn_12.owfn \
            error_parser_owfn_13.owfn error_parser_owfn_14.owfn \
            error_parser_owfn_15.owfn error_parser_owfn_16.owfn \
            error_parser_owfn_17.owfn error_parser_owfn_18.owfn \
            error_parser_owfn_19.owfn error_parser_owfn_20.owfn \
            error_parser_woflan_01.tpn error_parser_woflan_02.tpn \
            error_parser_woflan_03.tpn error_parser_woflan_04.tpn \
            error_parser_woflan_05.tpn error_parser_woflan_06.tpn \
            woflan1.xml.tpn woflan2.xml.tpn woflan3.xml.tpn \
            woflan4.xml.tpn woflan5.xml.tpn woflan6.xml.tpn \
            test_compose_ports_1.owfn test_compose_ports_2.owfn \
            test_cdnf.owfn test_input.lola test_dead_nodes.net.owfn \
            test_input.ifn \
            test_dnf.owfn test_identical_places.net.owfn \
            test_identical_transitions.net.owfn test_input_roles.owfn \
            test_input_roles_error.owfn test_self_loop_places.net.owfn \
            test_self_loop_transitions.net.owfn test_series_places.net.owfn \
            test_series_transitions.net.owfn \
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
            syntax_example.owfn syntax_example_new.owfn \
            syntax_example_new_short.owfn \
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
            cyclicFullCoffee.owfn phils.pnml phils.lola phils.pnt ph.pnt \
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
            prom-Genetic.pnml prom-NonFittingModel.pnml \
            prom-componentnet2.pnml \
            prom-runningExample.pnml prom-Genetic_withLazyTasks.pnml \
            prom-NonFittingModel_artificialStartEndTasks.pnml prom-eric.pnml \
            prom-runningExample_Alpha.pnml prom-GoodModel.pnml \
            prom-Onderzoek_voor_Behandeling.pnml prom-erm_model.pnml \
            prom-runningExample_AlphaPlusPlus.pnml \
            prom-GoodModel_artificialStartEndTasks.pnml \
            prom-OverlyGeneral.pnml \
            prom-freechoicetelim.pnml prom-runningExample_petriy.pnml \
            prom-Heuristic.pnml prom-OverlyGeneralModel.pnml \
            prom-g24_alphaAlgorithm.pnml prom-test1.pnml \
            prom-HeuristicMiner.pnml \
            prom-OverlyGeneralModel_artificialStartEndTasks.pnml \
            prom-g24_alphaPlusPlusAlgorithm.pnml prom-testnet.pnml \
            prom-Heuristic_withLazyTasks.pnml prom-OverlyPreciseModel.pnml \
            prom-originalModel.pnml N0.owfn N1.owfn N2.owfn N3.owfn \
            travel.owfn travel2.owfn prom-bla.pnml finalcondition_only.owfn


#-----------------------------------------#
# Part 2: Exclusions for the Cover Script #
#-----------------------------------------#

# Below you can list all the files you do not want to have counted in the
# coverability report that can be created by calling "make cover". Usually,
# these files are generated code or helper libraries that have not much to
# do with your tool.
# Absolute filenames are expected, but you may use '*' as wildcard. For
# example '*/libs/*' ignores everything in the 'libs' directory and
# '*/cmdline.c' will exclude any file in the sources called 'cmdline.c'.
# NO_COVER_FILES = */readnet-lexic.cc */readnet-syntax.cc */cmdline.*
NO_COVER_FILES = */cmdline.* */*-cmdline.* */parser-*.cc */parser-*.h */FlexLexer.h


#--------------------------------------------------------#
# Part 3: Generics -- YOU MUST NOT CHANGE ANYTHING BELOW #
#--------------------------------------------------------#

include $(top_srcdir)/m4/Makefile.tests

