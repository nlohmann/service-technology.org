# the main program - not bin_PROGRAMS as installation is handled manually
bin_PROGRAMS = lola2

# the plain sources you need to compile (no generated code)
lola2_SOURCES = main.cc \
                Dimensions.h \
                Socket.cc Socket.h \
                SymbolTable.cc SymbolTable.h \
                Symbol.cc Symbol.h

AM_CFLAGS = -Wall -W -Wextra -pedantic
AM_CXXFLAGS = -Wall -W -Wextra -pedantic
AM_LDFLAGS = -Wall -W -Wextra -pedantic


#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
