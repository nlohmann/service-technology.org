# the main program
bin_PROGRAMS = lola

#lola_CFLAGS = -g -ggdb -pedantic
#lola_CXXFLAGS = -g -ggdb -pedantic

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                $(KCSOURCES) \
                cmdline.c cmdline.h \
                \
                Core/Dimensions.h \
                Core/Handlers.cc Core/Handlers.h \
                Core/main.cc \
                \
				Parser/Parser/error.cc Parser/Parser/error.h \
                Parser/Parser/ParserPTNet.h Parser/Parser/ParserPTNet.cc \
                Parser/Parser/ParserPTNetLoLA.yy Parser/Parser/Lexic.ll \
                Parser/Parser/ParserPTFormula.yy Parser/Parser/LexicFormula.ll \
                Parser/Parser/ParserPTBuechi.yy Parser/Parser/LexicBuechi.ll \
				\
                Parser/SymbolTable/ArcList.cc Parser/SymbolTable/ArcList.h \
                Parser/SymbolTable/PlaceSymbol.cc Parser/SymbolTable/PlaceSymbol.h \
                Parser/SymbolTable/Symbol.cc Parser/SymbolTable/Symbol.h \
                Parser/SymbolTable/SymbolTable.cc Parser/SymbolTable/SymbolTable.h \
                Parser/SymbolTable/TransitionSymbol.h Parser/SymbolTable/TransitionSymbol.cc \
                \
                Net/LinearAlgebra.cc Net/LinearAlgebra.h \
                Net/Marking.cc Net/Marking.h \
                Net/Net.cc Net/Net.h \
                Net/NetState.cc Net/NetState.h \
                Net/Place.cc Net/Place.h \
                Net/Transition.cc Net/Transition.h \
                \
                InputOutput/CompressedIO.cc InputOutput/CompressedIO.h \
                InputOutput/InputOutput.cc InputOutput/InputOutput.h \
                InputOutput/Reporter.cc InputOutput/Reporter.h \
                InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                InputOutput/Socket.cc InputOutput/Socket.h \
                \
                Planning/Task.cc Planning/Task.h \
                \
                Exploration/CTLExploration.cc Exploration/CTLExploration.h \
                Exploration/LTLExploration.cc Exploration/LTLExploration.h Exploration/LTLStack.h \
                Exploration/DFSExploration.cc Exploration/DFSExploration.h \
                Exploration/ParallelExploration.cc Exploration/ParallelExploration.h \
                Exploration/Firelist.cc Exploration/Firelist.h \
                Exploration/RandomWalk.cc Exploration/RandomWalk.h \
                Exploration/SearchStack.h \
                Exploration/SimpleProperty.h \
                Exploration/Deadlock.cc Exploration/Deadlock.h \
                Exploration/ChooseTransition.h \
                Exploration/ChooseTransitionHashDriven.cc Exploration/ChooseTransitionHashDriven.h \
                Exploration/StatePredicateProperty.cc Exploration/StatePredicateProperty.h \
                Exploration/FirelistStubbornDeadlock.cc Exploration/FirelistStubbornDeadlock.h \
                Exploration/FirelistStubbornDeletion.cc Exploration/FirelistStubbornDeletion.h \
                Exploration/FirelistStubbornStatePredicate.cc Exploration/FirelistStubbornStatePredicate.h \
                \
                Formula/StatePredicate.h Formula/StatePredicate.cc\
                Formula/AtomicStatePredicate.cc Formula/AtomicStatePredicate.h \
                Formula/ConjunctionStatePredicate.cc Formula/ConjunctionStatePredicate.h \
                Formula/DisjunctionStatePredicate.cc Formula/DisjunctionStatePredicate.h \
                Formula/TruePredicate.cc Formula/TruePredicate.h \
                Formula/FalsePredicate.cc Formula/FalsePredicate.h \
                Formula/LTL/BuechiAutomata.h Formula/LTL/BuechiAutomata.cc \
                Formula/LTL/BuechiFromLTL.h Formula/LTL/ltl2ba.h \
                Formula/LTL/trans.c Formula/LTL/buchi.c Formula/LTL/main.c \
                Formula/LTL/alternating.c Formula/LTL/mem.c Formula/LTL/set.c \
                Formula/LTL/generalized.c Formula/LTL/cache.c Formula/LTL/lex.c \
                Formula/LTL/rewrt.c Formula/LTL/parse.c \
                Formula/CTL/CTLFormula.h \
                Formula/CTL/DFSFormula.h \
                Formula/CTL/ConjunctionFormula.h \
                Formula/CTL/DisjunctionFormula.h \
                Formula/CTL/NotFormula.h \
                Formula/CTL/TrueFormula.h \
                Formula/CTL/AtomicFormula.h \
                Formula/CTL/AFFormula.h \
                Formula/CTL/AGFormula.h \
                Formula/CTL/ARFormula.h \
                Formula/CTL/AXFormula.h \
                Formula/CTL/EFFormula.h \
                Formula/CTL/EGFormula.h \
                Formula/CTL/ERFormula.h \
                Formula/CTL/EXFormula.h \
                Formula/CTL/AUFormula.h Formula/CTL/AUFormula.cc \
                Formula/CTL/EUFormula.h Formula/CTL/EUFormula.cc \
                \
                Stores/Store.h Stores/Store.inc \
                Stores/EmptyStore.h \
                Stores/CompareStore.h Stores/CompareStore.inc \
                Stores/CycleStore.h \
                Stores/PluginStore.h Stores/PluginStore.inc \
                Stores/NetStateEncoder/NetStateEncoder.h Stores/NetStateEncoder/NetStateEncoder.cc \
                Stores/NetStateEncoder/BitEncoder.cc Stores/NetStateEncoder/BitEncoder.h \
                Stores/NetStateEncoder/CopyEncoder.cc Stores/NetStateEncoder/CopyEncoder.h \
                Stores/NetStateEncoder/FullCopyEncoder.cc Stores/NetStateEncoder/FullCopyEncoder.h \
                Stores/NetStateEncoder/SimpleCompressedEncoder.cc Stores/NetStateEncoder/SimpleCompressedEncoder.h \
                Stores/VectorStores/VectorStore.h \
                Stores/VectorStores/HashingWrapperStore.h Stores/VectorStores/HashingWrapperStore.inc \
                Stores/VectorStores/VSTLStore.cc Stores/VectorStores/VSTLStore.h Stores/VectorStores/VSTLStore.inc \
                Stores/VectorStores/VBloomStore.h Stores/VectorStores/VBloomStore.inc \
                Stores/VectorStores/PrefixTreeStore.cc Stores/VectorStores/PrefixTreeStore.h Stores/VectorStores/PrefixTreeStore.inc \
                \
                SweepLine/Sweep.h SweepLine/Sweep.inc \
                SweepLine/SweepRingStore.h SweepLine/SweepRingStore.inc \
                SweepLine/SweepListStore.h SweepLine/SweepListStore.inc \
                SweepLine/SweepEmptyStore.h SweepLine/SweepEmptyStore.cc \
                \
                Witness/Event.h Witness/Event.cc Witness/Condition.h Witness/Condition.cc

###

noinst_PROGRAMS = listener killer

listener_SOURCES = utils/Listener.cc \
                   InputOutput/Reporter.cc InputOutput/Reporter.h \
                   InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                   InputOutput/Socket.cc InputOutput/Socket.h

killer_SOURCES = utils/Killer.cc \
                 InputOutput/Reporter.cc InputOutput/Reporter.h \
                 InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                 InputOutput/Socket.cc InputOutput/Socket.h



#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src


##########################################
# everything about the Kimwitu++ sources #
##########################################

# define flags for Kimwitu++:
#  - create a file "ast-system-yystype.h" for Flex and Bison
#  - prefix all generated files with "ast-system-"
#  - suffix all generated files with "cc"
#  - do not create functions for phylum I/O
KCFLAGS = --yystype --file-prefix=ast-system- --suffix=cc --no-csgio

# a dummy file to use Makefile's timestamps
KCDUMMY = Parser/Parser/.timestamp

BUILT_SOURCES += $(KCDUMMY)


# Files that create C++ files that are not linked to the executable. These
# sources can be deleted before compilation (usually the unparsers).
KCFILES_TEMP =

# files that create C++ that have to be linked to executable
KCFILES_NECESSARY = Parser/Parser/formula_abstract.k Parser/Parser/formula_rewrite.k Parser/Parser/formula_unparse.k

# source files generated by Kimwitu++ to be distributed
KCSOURCES = Parser/Parser/ast-system-k.cc Parser/Parser/ast-system-k.h \
	Parser/Parser/ast-system-rk.cc Parser/Parser/ast-system-rk.h \
	Parser/Parser/ast-system-unpk.cc Parser/Parser/ast-system-unpk.h \
	$(KCFILES_NECESSARY:.k=.cc) \
	$(KCFILES_NECESSARY:.k=.h) \
	Parser/Parser/ast-system-yystype.h

# Dependencies of the Kimwitu++ files: the timestamp is renewed when a
# Kimwitu++ file is touched. This triggers the creation of all source files.
# The sed-script fixes problems occurring when BPEL2oWFN is compiled on
# x86_64 machines.
$(KCDUMMY): $(KCFILES_NECESSARY) $(KCFILES_TEMP)
	$(AM_V_at)touch $@
	$(AM_V_GEN)$(KC) $(KCFLAGS) $^
	$(AM_V_at)$(SED) -i 's/%x", reinterpret_cast<int>/%lx", reinterpret_cast<unsigned long int>/g' ast-system-k.cc
	$(AM_V_at)$(SED) -i 's/%x_%d", reinterpret_cast<int>/%lx_%d", reinterpret_cast<unsigned long int>/g' ast-system-k.cc
	$(AM_V_at)mv $(KCSOURCES:Parser/Parser/%=%) Parser/Parser

MAINTAINERCLEANFILES += $(KCSOURCES) $(KCDUMMY)

EXTRA_DIST += $(KCDUMMY) $(KCFILES_TEMP) $(KCFILES_NECESSARY)
