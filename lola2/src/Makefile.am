# the main program
bin_PROGRAMS = lola

lola_CFLAGS = -g
lola_CXXFLAGS = -g

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                $(KCSOURCES) \
                cmdline.c cmdline.h \
                \
                Core/Dimensions.h \
                Core/Handlers.cc Core/Handlers.h \
                Core/main.cc \
                \
                Parser/ArcList.cc Parser/ArcList.h \
                Parser/FairnessAssumptions.h \
                Parser/ParserPTNet.h Parser/ParserPTNet.cc \
                Parser/ParserPTNetLoLA.yy Parser/Lexic.ll \
                Parser/ParserPTFormula.yy Parser/LexicFormula.ll \
                Parser/PlaceSymbol.cc Parser/PlaceSymbol.h \
                Parser/Symbol.cc Parser/Symbol.h \
                Parser/SymbolTable.cc Parser/SymbolTable.h \
                Parser/TransitionSymbol.h Parser/TransitionSymbol.cc \
                \
                Net/swap.h Net/swap.cc \
                Net/LinearAlgebra.cc Net/LinearAlgebra.h \
                Net/Marking.cc Net/Marking.h \
                Net/Net.cc Net/Net.h \
               	Net/NetState.cc Net/NetState.h \
                Net/Place.cc Net/Place.h \
                Net/StructuralReduction.cc Net/StructuralReduction.h \
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
                Exploration/Firelist.cc Exploration/Firelist.h \
                Exploration/RandomWalk.cc Exploration/RandomWalk.h \
                Exploration/SearchStack.cc Exploration/SearchStack.h \
                Exploration/SimpleProperty.cc Exploration/SimpleProperty.h \
                Exploration/ParallelSimpleProperty.cc Exploration/ParallelSimpleProperty.h \
                Exploration/Deadlock.cc Exploration/Deadlock.h \
                Exploration/ChooseTransition.h \
                Exploration/ChooseTransitionHashDriven.cc Exploration/ChooseTransitionHashDriven.h \
                Exploration/StatePredicateProperty.cc Exploration/StatePredicateProperty.h \
                Exploration/FirelistStubbornDeadlock.cc Exploration/FirelistStubbornDeadlock.h \
                Exploration/FirelistStubbornStatePredicate.cc Exploration/FirelistStubbornStatePredicate.h \
                \
                Formula/StatePredicate.h Formula/StatePredicate.cc\
                Formula/AtomicStatePredicate.cc Formula/AtomicStatePredicate.h \
                Formula/ConjunctionStatePredicate.cc Formula/ConjunctionStatePredicate.h \
                Formula/DisjunctionStatePredicate.cc Formula/DisjunctionStatePredicate.h \
                Formula/TruePredicate.cc Formula/TruePredicate.h \
                Formula/FalsePredicate.cc Formula/FalsePredicate.h \
                \
                Stores/Store.cc Stores/Store.h \
                Stores/SIStore.h \
                Stores/BloomStore.cc Stores/BloomStore.h \
                Stores/BinStore.h Stores/BinStore.cc \
                Stores/BitStore.cc Stores/BitStore.h \
                Stores/EmptyStore.cc Stores/EmptyStore.h \
                Stores/STLStore.cc Stores/STLStore.h \
                Stores/ThreadSafeStore.cc Stores/ThreadSafeStore.h \
                Stores/BinStore2.cc Stores/BinStore2.h \
                Stores/SIBinStore2.cc Stores/SIBinStore2.h

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
KCDUMMY = Parser/.timestamp

BUILT_SOURCES += $(KCDUMMY)


# Files that create C++ files that are not linked to the executable. These
# sources can be deleted before compilation (usually the unparsers).
KCFILES_TEMP =

# files that create C++ that have to be linked to executable
KCFILES_NECESSARY = Parser/formula_abstract.k Parser/formula_rewrite.k Parser/formula_unparse.k

# source files generated by Kimwitu++ to be distributed
KCSOURCES = Parser/ast-system-k.cc Parser/ast-system-k.h \
	Parser/ast-system-rk.cc Parser/ast-system-rk.h \
	Parser/ast-system-unpk.cc Parser/ast-system-unpk.h \
	$(KCFILES_NECESSARY:.k=.cc) \
	$(KCFILES_NECESSARY:.k=.h) \
	Parser/ast-system-yystype.h

# Dependencies of the Kimwitu++ files: the timestamp is renewed when a
# Kimwitu++ file is touched. This triggers the creation of all source files.
# The sed-script fixes problems occurring when BPEL2oWFN is compiled on
# x86_64 machines.
$(KCDUMMY): $(KCFILES_NECESSARY) $(KCFILES_TEMP)
	$(AM_V_at)touch $@
	$(AM_V_GEN)$(KC) $(KCFLAGS) $^
	$(AM_V_at)$(SED) -i 's/reinterpret_cast<int>/reinterpret_cast<size_t>/g' ast-system-k.cc
	$(AM_V_at)mv $(KCSOURCES:Parser/%=%) Parser

MAINTAINERCLEANFILES += $(KCSOURCES) $(KCDUMMY)

EXTRA_DIST += $(KCDUMMY) $(KCFILES_TEMP) $(KCFILES_NECESSARY)
