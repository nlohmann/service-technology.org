# Script to check for soundness and safeness, as well as to generate
# tables for the data. To use, check the values of the following
# variables. Type "make help" for help.

LIBRARY           = b3
SED               = sed                # sed
LOLA-SP           = lola-bpm-statepredicate1 # LoLA with STATEPREDICATE
LOLA-MC           = lola-bpm-modelchecking1  # LoLA with MODELCHECKING
LOLA-LP           = lola-bpm-liveprop1       # LoLA with LIVEPROP

REDUCTION-LEVEL   = 0                  # Petri net reduction level for the
                                       # translation
UML2OWFN-PARAM    = -s empty -p ctl    # additional UML2oWFN paramters
OUTPUT-FORMAT     = lola               # output format to be produced by UML2oWFN
                                       # see uml2owfn -h for feasible options
                                       
LOLA_DIR          =
UML2OWFN_DIR      =
                                       
##########################################################################


##########################################################################
LOLAFILES = $(wildcard *.lola)

.SUFFIXES: .lola .lola.safe.output .lola.sound.output .lola.full.output

help:
	@echo
	@echo "targets:"
	@echo "* translate      translate library"
	@echo "* translateweak  translate library with weaker termination semantics"
	@echo "* translatewf    translate library into classical workflow nets"
	@echo "* translateor    translate library nets with OR-join termination"
	@echo "* soundness      soundness check (AGEF omega)"
	@echo "* safeness       safeness check"
	@echo "* table          collect results in table"
	@echo "* clean          remove analysis files"
	@echo "* veryclean      remove Petri nets"
	@echo

soundness: $(LOLAFILES:.lola=.lola.sound.output)
safeness:  $(LOLAFILES:.lola=.lola.safe.output)
full:      $(LOLAFILES:.lola=.lola.full.output)


##########################################################################
# analysis (safeness, soundness, full state space)
##########################################################################
%.lola.safe.output: %.lola
	-@$(LOLA_DIR)$(LOLA-SP) $< -a$(<:.lola=.lola.safe.task) > $@ 2> $@ ; echo SAFENESS-RESULT: $$? >> $@

%.lola.sound.output: %.lola
	-@$(LOLA_DIR)$(LOLA-MC) $< -a$(<:.lola=.lola.fin.task) > $@ 2> $@ ; echo SOUNDNESS-RESULT: $$? >> $@


##########################################################################
# table
##########################################################################

table: results.csv

results.csv: safeness soundness
	# collect the names and extract the process names
	ls -1 *.lola | $(SED) -e 's/_/ /g;s/$(LIBRARY).//;s/.lola//' | awk {'print $$2'} > result_NAMES.log
	# collect the results from the soundness analysis
	grep "SOUNDNESS-RESULT" *.sound.output | awk {' print $$2 '} > result_SOUNDNESS.log
	$(SED) -i 's/0/sound/;s/1/unsound/;s/4/unsafe/' result_SOUNDNESS.log
	# collect the results from the safeness analysis
	grep "SAFENESS-RESULT" *.safe.output | awk {' print $$2 '} > result_SAFENESS.log
	$(SED) -i 's/0/unsafe/;s/1/safe/;s/4/unsafe/' result_SAFENESS.log
	# collect the size of the nets
	grep "|P|" translation.log | awk {'print $$1'} | $(SED) -e 's/|P|=//;s/,//' > result_PLACE.log
	grep "|T|" translation.log | awk {'print $$6'} | $(SED) -e 's/|T|=//;s/,//' > result_TRANSITION.log
	# collect the states (if 0, take the hash table entries)
	grep "States" *.sound.output | awk {'if ($$2 == 0) print $$6; else print $$2'} > result_STATES.log
	grep "States" *.full.output | awk {'print $$2'} > result_STATES_FULL.log
	# make a table out of the intermediate results
	paste -d';' \
		result_NAMES.log \
		result_SOUNDNESS.log \
		result_SAFENESS.log \
		result_PLACE.log \
		result_TRANSITION.log \
		result_STATES.log \
		result_STATES_FULL.log \
		| sort > results.csv
	# consolidate analysis results
	$(SED) -i 's/sound,unsafe/unsafe/;s/unsound,safe/unsound/;s/sound,safe/sound/;s/unsafe,unsafe/unsafe/' results.csv
	$(SED) -i '1iProcess Name,Result,Places,Transitions,States seen,States total' results.csv
	# table for comparison
	paste -d';' \
		result_NAMES.log \
		result_SOUNDNESS.log \
		result_SAFENESS.log \
		| sort > results-comparison.csv
	$(SED) -i 's/,/ : /' results-comparison.csv
	$(SED) -i 's/unsound,unsafe/udl-uls/' results-comparison.csv
	$(SED) -i 's/unsound,safe/udl/'       results-comparison.csv
	$(SED) -i 's/unsafe,unsafe/uls/'      results-comparison.csv
	$(SED) -i 's/unsafe,safe/ERROR/'      results-comparison.csv
	$(SED) -i 's/sound,unsafe/uls/'       results-comparison.csv
	$(SED) -i 's/sound,safe/s/'           results-comparison.csv
	# remove intermediate results
	#rm result_NAMES.log result_SOUNDNESS.log result_SAFENESS.log result_PLACE.log result_TRANSITION.log result_STATES.log result_STATES_FULL.log


##########################################################################
# create files
##########################################################################
translate: $(LIBRARY).xml
	$(UML2OWFN_DIR)uml2owfn -i $< -f ${OUTPUT-FORMAT} -a soundness -a safe -p taskfile -p log -r $(REDUCTION-LEVEL) $(UML2OWFN-PARAM) -o 2> translation.log

translateweak: $(LIBRARY).xml
	$(UML2OWFN_DIR)uml2owfn -i $< -f ${OUTPUT-FORMAT} -a soundness -a safe -a noData -p taskfile -p log -r $(REDUCTION-LEVEL) $(UML2OWFN-PARAM) -o 2> translation.log
	
translatewf: $(LIBRARY).xml
	$(UML2OWFN_DIR)uml2owfn -i $< -f ${OUTPUT-FORMAT} -a soundness -a safe -a wfNet -p taskfile -p log -r $(REDUCTION-LEVEL) $(UML2OWFN-PARAM) -o 2> translation.log

translateor: $(LIBRARY).xml
	$(UML2OWFN_DIR)uml2owfn -i $< -f ${OUTPUT-FORMAT} -a soundness -a safe -a orJoin -p taskfile -p log -r $(REDUCTION-LEVEL) $(UML2OWFN-PARAM) -o 2> translation.log


##########################################################################
# tidy up
##########################################################################
clean:
	-rm *.lola.sound.output
	-rm *.lola.safe.output
	-rm *.lola.full.output

veryclean: clean
	-rm *.lola *.task *.log check_*.sh results.csv results-comparison.csv


##########################################################################
# package
##########################################################################

dist:
	@tar -pczf $(LIBRARY)-dist.tar.gz .
	@echo create copy in $(LIBRARY)-dist.tar.gz
