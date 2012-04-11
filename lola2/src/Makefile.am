# the main program
bin_PROGRAMS = lola listener killer

lola_CFLAGS = -g
lola_CXXFLAGS = -g

# the plain sources you need to compile (no generated code)
lola_SOURCES = \
                ArcList.cc ArcList.h \
                BinStore.h BinStore.cc \
                BitStore.cc BitStore.h \
                BDDStore.cc BDDStore.h \
                CompressedIO.cc CompressedIO.h \
                Dimensions.h \
                FairnessAssumptions.h \
                Firelist.cc Firelist.h \
                Handlers.cc Handlers.h \
                InputOutput.cc InputOutput.h \
                Marking.cc Marking.h \
                Net.cc Net.h \
                ParserPTNet.h ParserPTNet.cc \
                ParserPTNetLoLA.yy Lexic.ll \
                Place.cc Place.h \
                PlaceSymbol.cc PlaceSymbol.h \
                RandomWalk.cc RandomWalk.h \
                Reporter.cc Reporter.h \
                LinearAlgebra.cc LinearAlgebra.h \
                SearchStack.cc SearchStack.h \
                SimpleProperty.cc SimpleProperty.h \
                StructuralReduction.cc StructuralReduction.h \
                Socket.cc Socket.h \
                STLStore.cc STLStore.h \
                SQLStore.cc SQLStore.h \
                SimpleString.cc SimpleString.h \
                Store.cc Store.h \
                swap.h swap.cc \
                Symbol.cc Symbol.h \
                SymbolTable.cc SymbolTable.h \
                Transition.cc Transition.h \
                TransitionSymbol.h TransitionSymbol.cc \
                cmdline.c cmdline.h \
                main.cc

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
                   Reporter.cc Reporter.h \
                   SimpleString.cc SimpleString.h \
                   Socket.cc Socket.h

killer_SOURCES = Killer.cc \
                 Reporter.cc Reporter.h \
                 SimpleString.cc SimpleString.h \
                 Socket.cc Socket.h

#############################################################################
# EVERYTHING BELOW THIS LINE IS GENERIC - YOU MUST NOT CHANGE ANYTHING BELOW
#############################################################################

include $(top_srcdir)/m4/Makefile.src
