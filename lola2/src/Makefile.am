# the main program
bin_PROGRAMS = lola listener killer

lola_CFLAGS = -g
lola_CXXFLAGS = -g

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                Dimensions.h \
                Firelist.cc Firelist.h \
                Handlers.cc Handlers.h \
                RandomWalk.cc RandomWalk.h \
                SearchStack.cc SearchStack.h \
                SimpleProperty.cc SimpleProperty.h \
                cmdline.c cmdline.h \
                main.cc \
                \
                Net/swap.h Net/swap.cc \
                Net/FairnessAssumptions.h \
                Net/LinearAlgebra.cc Net/LinearAlgebra.h \
                Net/Marking.cc Net/Marking.h \
                Net/Net.cc Net/Net.h \
                Net/Place.cc Net/Place.h \
                Net/StructuralReduction.cc Net/StructuralReduction.h \
                Net/Transition.cc Net/Transition.h \
                \
                Parser/ArcList.cc Parser/ArcList.h \
                Parser/ParserPTNet.h Parser/ParserPTNet.cc \
                Parser/ParserPTNetLoLA.yy Parser/Lexic.ll \
                Parser/PlaceSymbol.cc Parser/PlaceSymbol.h \
                Parser/Symbol.cc Parser/Symbol.h \
                Parser/SymbolTable.cc Parser/SymbolTable.h \
                Parser/TransitionSymbol.h Parser/TransitionSymbol.cc \
                \
                InputOutput/CompressedIO.cc InputOutput/CompressedIO.h \
                InputOutput/InputOutput.cc InputOutput/InputOutput.h \
                InputOutput/Reporter.cc InputOutput/Reporter.h \
                InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                InputOutput/Socket.cc InputOutput/Socket.h \
                \
                Stores/Store.cc Stores/Store.h \
                Stores/BinStore.h Stores/BinStore.cc \
                Stores/BitStore.cc Stores/BitStore.h \
                Stores/BDDStore.cc Stores/BDDStore.h \
                Stores/STLStore.cc Stores/STLStore.h \
                Stores/SQLStore.cc Stores/SQLStore.h

lola_LDADD = $(builddir)/../libs/cudd/libobj.a \
	$(builddir)/../libs/cudd/libdddmp.a \
	$(builddir)/../libs/cudd/libcudd.a \
	$(builddir)/../libs/cudd/libmtr.a \
	$(builddir)/../libs/cudd/libst.a \
	$(builddir)/../libs/cudd/libutil.a \
	$(builddir)/../libs/cudd/libepd.a \
	$(builddir)/../libs/sqlite/libsqlite.a

lola_CPPFLAGS = -I$(top_srcdir)/libs/cudd/include -I$(top_srcdir)/libs/sqlite

listener_SOURCES = Listener.cc \
                   InputOutput/Reporter.cc InputOutput/Reporter.h \
                   InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                   InputOutput/Socket.cc InputOutput/Socket.h

killer_SOURCES = Killer.cc \
                 InputOutput/Reporter.cc InputOutput/Reporter.h \
                 InputOutput/SimpleString.cc InputOutput/SimpleString.h \
                 InputOutput/Socket.cc InputOutput/Socket.h

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
