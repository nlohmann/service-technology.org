# Script to check for soundness and safeness, as well as to generate
# tables for the data. To use, check the values of the following
# variables. Type "make help" for help.

LIBRARY           = b3
SED               = gsed               # GNU sed
LOLA-SP           = lola-sp            # LoLA with STATEPREDICATE
LOLA-MC           = lola-mc            # LoLA with MODELCHECKING
LOLA-FULL-LIMITED = lola-full-limited  # LoLA with FULL
##########################################################################


##########################################################################
LOLAFILES = $(wildcard *.lola)

.SUFFIXES: .lola .lola.safe.output .lola.sound.output .lola.full.output

help:
	@echo
	@echo "targets:"
	@echo "* translate     translate library"
	@echo "* soundness     soundness check (AGEF omega)"
	@echo "* safeness      safeness check"
	@echo "* full          full state space"
	@echo "* table         collect results in table"
	@echo "* clean         remove analysis files"
	@echo "* veryclean     remove Petri nets"
	@echo

soundness: $(LOLAFILES:.lola=.lola.sound.output)
safeness:  $(LOLAFILES:.lola=.lola.safe.output)
full:      $(LOLAFILES:.lola=.lola.full.output)


##########################################################################
# analysis (safeness, soundness, full state space)
##########################################################################
%.lola.safe.output: %.lola
	-@$(LOLA-SP) $< -a $(<:.lola=.lola.safe.task) > $@ 2> $@ ; echo SAFENESS-RESULT: $$? >> $@

%.lola.sound.output: %.lola
	-@$(LOLA-MC) $< -a $(<:.lola=.lola.fin.task) > $@ 2> $@ ; echo SOUNDNESS-RESULT: $$? >> $@

%.lola.full.output: %.lola
	-@$(LOLA-FULL-LIMITED) $< > $@ 2> $@ || echo FULL-RESULT: $$? >> $@
	@$(SED) -i 's/FULL-RESULT: 5/FULL-RESULT: 5\n>>>>> oo States/' $@


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
	paste -d',' \
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
	paste -d',' \
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
	rm result_NAMES.log result_SOUNDNESS.log result_SAFENESS.log result_PLACE.log result_TRANSITION.log result_STATES.log result_STATES_FULL.log


##########################################################################
# create files
##########################################################################
translate: $(LIBRARY).xml
	uml2owfn -i $< -f lola -a soundness -a safe -p taskfile -p filter -p log -r 1 -o 2> translation.log


##########################################################################
# tidy up
##########################################################################
clean:
	-rm *.lola.sound.output
	-rm *.lola.safe.output
	-rm *.lola.full.output

veryclean: clean
	-rm *.lola *.task *.log results.csv results-comparison.csv


##########################################################################
# package
##########################################################################

dist:
	@tar -pczf $(LIBRARY)-dist.tar.gz .
	@echo create copy in $(LIBRARY)-dist.tar.gz
