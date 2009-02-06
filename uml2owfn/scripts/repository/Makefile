# Script to analyse nets and collect important data in tables

UML2OWFN       = uml2owfn
UML2OWFN_FLAGS = -p log
LOLA           = lola
LOLA-MC        = lola-mc
LOLA-SP        = lola-sp
LOLA-FULL-LIMITED = lola-full-limited
FIONA          = fiona
MAKE           = make
#BPELSCRIPT     = ./bpel_info.sh
BPELSCRIPT     = echo
#PETRINETSCRIPT = ./petrinet_info.sh
PETRINETSCRIPT = echo
SOUNDNESSSCRIPT = ./soundness_info.sh
CONTROLLABILITYSCRIPT = ./controllability_info.sh

SED            = sed

MKDIR_P        = mkdir -p

#############################################################

UMLFILES = $(wildcard */*.xml)

DIRS      = $(wildcard */)

all: table

help:
	@echo
	@echo "targets:"
	@echo "* all        careless all around"
	@echo "* table      collects results in tables"
	@echo "* files      translate all source files into Petri nets"
	@echo "* dist       packs tables to tar-ball"
	@echo "* outfiles   generates all necessary output files"
	@echo "* clean      cleans up outfiles"
	@echo "* cleanall   cleans up outfiles, html- and tex-files"
	@echo ""
	@echo "* soundness  run all soundness analysis tasks"
	@echo "* controllability  run all controllability analysis tasks"
	@echo

# list of all make targets to verify soundness
SOUNDNESS_TASKS = sound_r sound_r_pin

define TARGET_template_SOUNDNESS
  # basic make target
  $(1): $$(UMLFILES:.xml=/$(1)/)
  # basic file lists
  LOLAFILES_$(1) := $$(wildcard */*/$(1)/*.lola)
  # make target file lists for individual analysis tasks
  lola_safe_$(1)_out: $(1) $$(LOLAFILES_$(1):.lola=.lola.safe.outfile)
  lola_sound_$(1)_out: $(1) $$(LOLAFILES_$(1):.lola=.lola.sound.outfile)
  lola_full_$(1)_out: $(1) $$(LOLAFILES_$(1):.lola=.lola.full.outfile)
  # make target file lists for composed analysis tasks
  lola_safesound_$(1)_out: $$(LOLAFILES_$(1):.lola=.lola.safesound)
  lola_soundsafe_$(1)_out: $$(LOLAFILES_$(1):.lola=.lola.soundsafe)
  # build target lists for different settings
  ALL_TARGETS_SOUNDNESS += lola_safe_$(1)_out lola_sound_$(1)_out
  ALL_TARGETS_SAFESOUND += lola_safesound_$(1)_out
  ALL_TARGETS_SOUNDSAFE += lola_soundsafe_$(1)_out
  ALL_TARGETS_COMPLETE += lola_full_$(1)_out lola_safe_$(1)_out lola_sound_$(1)_out
  # targets for creating tables
  $(1)_DIRS := $$(wildcard */*/$(1)/)
  $(1)_table: lola_sound_$(1)_out lola_safe_$(1)_out ; $$(MAKE) $(1)_tables_petrinet
  $(1)_tables_petrinet: $$($(1)_DIRS:/$(1)/=_$(1)_petrinet.html)
  %_$(1)_petrinet.html: lola_safe_$(1)_out lola_sound_$(1)_out ; @$(SOUNDNESSSCRIPT) $${@:_$(1)_petrinet.html=/$(1)/} $$@
  ALL_TARGETS_TABLE += $(1)_table
endef

$(foreach task,$(SOUNDNESS_TASKS),$(eval $(call TARGET_template_SOUNDNESS,$(task))))

CONTROLLABILITY_TASKS = ig_r ig_full

define TARGET_template_CONTROLLABILITY
  # basic make target
  $(1): $$(UMLFILES:.xml=/$(1)/)
  # basic file lists
  OWFNFILES_$(1) := $$(wildcard */*/$(1)/*.owfn)
  # make target file lists for individual analysis tasks
  owfn_$(1)_out: $(1) $$(OWFNFILES_$(1):.owfn=.owfn.outfile)
  owfn_$(1)_red_out: $(1) $$(OWFNFILES_$(1):.owfn=.owfn.outfile.red)
  # build target lists for different settings
  ALL_TARGETS_CONTROLLABILITY += owfn_$(1)_out owfn_$(1)_red_out
  ALL_TARGETS_COMPLETE += owfn_$(1)_out owfn_$(1)_red_out
  # targets for creating tables
  $(1)_DIRS := $$(wildcard */*/$(1)/)
  $(1)_table: owfn_$(1)_out owfn_$(1)_red_out ; $$(MAKE) $(1)_tables_petrinet
  $(1)_tables_petrinet: $$($(1)_DIRS:/$(1)/=_$(1)_petrinet.html)
  %_$(1)_petrinet.html: owfn_$(1)_out owfn_$(1)_red_out ; @$(CONTROLLABILITYSCRIPT) $${@:_$(1)_petrinet.html=/$(1)/} $$@
  ALL_TARGETS_TABLE += $(1)_table
endef

$(foreach task,$(CONTROLLABILITY_TASKS),$(eval $(call TARGET_template_CONTROLLABILITY,$(task))))


### generic make target definitions
### must be placed after configuration settings to set all variables value properly

files: $(SOUNDNESS_TASKS) $(CONTROLLABILITY_TASKS)

analysis: ${ALL_TARGETS_COMPLETE}
soundness: $(ALL_TARGETS_SOUNDNESS)
safesound: $(ALL_TARGETS_SAFESOUND)
soundsafe: $(ALL_TARGETS_SOUNDSAFE)

controllability: $(ALL_TARGETS_CONTROLLABILITY)

# the main make target that controls which results will be included in the
# produced tables, add your own @$(MAKE) __TARGET__ to the list to create
# call the desired analysis tasks

outfiles: files
	@$(MAKE) soundness
	@$(MAKE) controllability


##########################################################################
# library translation and preparation of sub-analysis
##########################################################################

%/sound_r/: %.xml
	-@$(MKDIR_P) ./$@/
	-@$(UML2OWFN) $(UML2OWFN_FLAGS) -i $< -r 3 -f lola -a soundness -a safe -a orJoin -p ctl -p taskfile --output=$@/ 2> $@/translation.log

%/sound_r_pin/: %.xml
	-@$(MKDIR_P) ./$@/
	-@$(UML2OWFN) $(UML2OWFN_FLAGS) -i $< -r 3 -f lola -a soundness -a safe -a noData -p ctl -p taskfile --output=$@/ 2> $@/translation.log

%/ig_r/: %.xml
	-@$(MKDIR_P) ./$@/
	-@$(UML2OWFN) $(UML2OWFN_FLAGS) -i $< -f owfn -r 3 -R --output=$@/ 2> $@/translation.log

%/ig_full/: %.xml
	-@$(MKDIR_P) ./$@/
	-@$(UML2OWFN) $(UML2OWFN_FLAGS) -i $< -f owfn -R --output=$@/ 2> $@/translation.log

##########################################################################
# analysis (safeness, soundness, full state space)
##########################################################################
%.lola.safe.outfile: %.lola
	-@$(LOLA-SP) $< -a $(<:.lola=.lola.safe.task) > $@ 2> $@ ; echo SAFENESS-RESULT: $$? >> $@

%.lola.sound.outfile: %.lola
	-@$(LOLA-MC) $< -a $(<:.lola=.lola.fin.task) > $@ 2> $@ ; echo SOUNDNESS-RESULT: $$? >> $@

%.lola.full.outfile: %.lola
	-@$(LOLA-FULL-LIMITED) $< > $@ 2> $@ || echo FULL-RESULT: $$? >> $@
	@$(SED) -i 's/FULL-RESULT: 5/FULL-RESULT: 5\n>>>>> oo States/' $@


# nur parsen
%.lola.parse: %.lola
	lola-none $<

# nur safeness falls sound
%.lola.safesound: %.lola
	-@ !($(LOLA-MC) $< -a $(<:.lola=.lola.fin.task)) || $(LOLA-SP) $< -a $(<:.lola=.lola.safe.task)

# nur soundness falls safe
%.lola.soundsafe: %.lola
	-@ $(LOLA-SP) $< -a $(<:.lola=.lola.safe.task) || $(LOLA-MC) $< -a $(<:.lola=.lola.fin.task)

##########################################################################
# analysis (controllability)
##########################################################################

%.owfn.outfile: %.owfn
	-@$(FIONA) $< -Q &> $@

%.owfn.outfile.red: %.owfn
	-@$(FIONA) -r $< -Q &> $@

##########################################################################
# table
##########################################################################

#   @$(MAKE) $(DIRS:/=_uml.html)
table: $(ALL_TARGETS_TABLE)

%_uml.html: %/
	-@$(BPELSCRIPT) $<

%_petrinet.html: %/
	-@$(PETRINETSCRIPT) $<

clean:
	@echo "the following clean targets are available:"
	@echo "* cleanall        all produced files, html- and tex-files"
	@echo "* cleananalysis   analysis results"
	@echo "* cleanfiles      generated model files and analysis results"
	@echo "* cleantable      generated tables, html- and tex-files"
	@echo

cleanfiles: cleananalysis
	-@rm */*/*/*.lola
	-@rm */*/*/*.owfn
	-@rm */*/*/*.task
	-@rm */*/*/translation.log
	-@rm */*/*/check_*.sh
	-@rm */*/*/uml2owfn_*.log
	-@rmdir */*/sound_r/
	-@rmdir */*/sound_r_pin/
	-@rmdir */*/ig_r/
	-@rmdir */*/ig_full/

cleananalysis: cleantemp
	-@rm */*/*/*.outfile*

cleantable: cleantemp
	-@rm */*.html
	-@rm */*.tex

cleantemp:
	-@rm */*/*/results.csv
	-@rm */*/*/result_*.log

cleanall: cleanfiles cleantable
	-@rm *.tar.gz

dist: table
	-@tar cvzf tables.tar.gz *.html *.tex
