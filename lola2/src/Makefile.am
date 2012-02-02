# the main program - not bin_PROGRAMS as installation is handled manually
bin_PROGRAMS = lola listener

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                ArcList.cc ArcList.h \
                BitSetC.cc BitSetC.h \
                Dimensions.h \
                FairnessAssumptions.h \
                Marking.cc Marking.h \
                Net.cc Net.h \
                ParserPTNet.h ParserPTNet.cc \
                ParserPTNetLoLA.yy Lexic.ll \
                Place.cc Place.h \
                PlaceSymbol.cc PlaceSymbol.h \
                RandomWalk.cc RandomWalk.h \
                Reporter.cc Reporter.h \
                Socket.cc Socket.h \
                Symbol.cc Symbol.h \
                SymbolTable2Net.cc SymbolTable2Net.h \
                SymbolTable.cc SymbolTable.h \
                Transition.cc Transition.h \
                TransitionSymbol.h TransitionSymbol.cc \
                cmdline.c cmdline.h \
                main.cc

listener_SOURCES = Listener.cc \
                   Reporter.cc Reporter.h \
                   Socket.cc Socket.h

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
