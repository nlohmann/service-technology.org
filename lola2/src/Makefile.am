# the main program - not bin_PROGRAMS as installation is handled manually
bin_PROGRAMS = lola

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                ArcList.cc ArcList.h \
                Dimensions.h \
                FairnessAssumptions.h \
                ParserPTNet.h \
                ParserPTNetLoLA.yy Lexik.ll \
                PlaceSymbol.cc PlaceSymbol.h \
                Reporter.cc Reporter.h \
                Socket.cc Socket.h \
                Symbol.cc Symbol.h \
                SymbolTable.cc SymbolTable.h \
                TransitionSymbol.h TransitionSymbol.cc \
                cmdline.c cmdline.h \
                main.cc

lola_CFLAGS = -Wall -W -Wextra -pedantic
lola_CXXFLAGS = -Wall -W -Wextra -pedantic
lola_LDFLAGS = -Wall -W -Wextra -pedantic


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
