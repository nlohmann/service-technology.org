bin_PROGRAMS = wendy

wendy_SOURCES = syntax_graph.yy lexic_graph.ll \
                main.cc types.h \
                FullMarking.h FullMarking.cc \
                InterfaceMarking.h InterfaceMarking.cc \
                Label.h Label.cc \
                InnerMarking.h InnerMarking.cc \
                Knowledge.h Knowledge.cc \
                StoredKnowledge.h StoredKnowledge.cc \
                verbose.cc verbose.h \
                cmdline.c cmdline.h

sysconf_DATA = wendy.conf

# pass the sysconfdir to Wendy
wendy_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
wendy_CPPFLAGS += -I$(top_srcdir)/libs
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
