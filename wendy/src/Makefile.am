bin_PROGRAMS = wendy

wendy_SOURCES = syntax_graph.yy lexic_graph.ll \
                main.cc types.h \
                FullMarking.h FullMarking.cc \
                InterfaceMarking.h InterfaceMarking.cc \
                Label.h Label.cc \
                InnerMarking.h InnerMarking.cc \
                Knowledge.h Knowledge.cc \
                StoredKnowledge.h StoredKnowledge.cc \
                cmdline.c cmdline.h

# only add the local Petri net API if necessary
if COMPILE_PNAPI
wendy_CPPFLAGS = -I$(top_srcdir)/libs
wendy_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a
endif

BUILT_SOURCES = cmdline.c syntax_graph.cc
EXTRA_DIST = syntax_graph.h cmdline.ggo
MAINTAINERCLEANFILES = syntax_graph.h cmdline.c cmdline.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
