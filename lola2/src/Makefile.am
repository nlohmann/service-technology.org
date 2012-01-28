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

AM_CFLAGS = -Waddress -Waggregate-return -Wall -Wcast-align -Wcast-qual -Wconversion -Wempty-body -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Winit-self -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-div-by-zero -Wno-endif-labels -Wno-format-extra-args -Wno-variadic-macros -Wpacked -Wpointer-arith -Wredundant-decls -Wundef -Wuninitialized -Wunused -Wwrite-strings -ansi -pedantic  -Wno-format
AM_CXXFLAGS = -Waddress -Waggregate-return -Wall -Wcast-align -Wcast-qual -Wconversion -Wempty-body -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Winit-self -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-div-by-zero -Wno-endif-labels -Wno-format-extra-args -Wno-variadic-macros -Wpacked -Wpointer-arith -Wredundant-decls -Wundef -Wuninitialized -Wunused -Wwrite-strings -ansi -pedantic  -Wno-format


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
