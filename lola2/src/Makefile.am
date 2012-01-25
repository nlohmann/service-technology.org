# the main program - not bin_PROGRAMS as installation is handled manually
bin_PROGRAMS = lola listener

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

listener_SOURCES = Listener.cc \
                   Reporter.cc Reporter.h \
                   Socket.cc Socket.h

lola_CFLAGS = -Waddress -Waggregate-return -Wall -Wcast-align -Wcast-qual -Wconversion -Wempty-body -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Winit-self -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-div-by-zero -Wno-endif-labels -Wno-format-extra-args -Wno-variadic-macros -Wpacked -Wpointer-arith -Wredundant-decls -Wundef -Wuninitialized -Wunused -Wwrite-strings -ansi -pedantic
lola_CXXFLAGS = -Waddress -Waggregate-return -Wall -Wcast-align -Wcast-qual -Wconversion -Wempty-body -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Winit-self -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-div-by-zero -Wno-endif-labels -Wno-format-extra-args -Wno-variadic-macros -Wpacked -Wpointer-arith -Wredundant-decls -Wundef -Wuninitialized -Wunused -Wwrite-strings -ansi -pedantic
lola_LDFLAGS = -Waddress -Waggregate-return -Wall -Wcast-align -Wcast-qual -Wconversion -Wempty-body -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Winit-self -Wmissing-field-initializers -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-div-by-zero -Wno-endif-labels -Wno-format-extra-args -Wno-variadic-macros -Wpacked -Wpointer-arith -Wredundant-decls -Wundef -Wuninitialized -Wunused -Wwrite-strings -ansi -pedantic


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
