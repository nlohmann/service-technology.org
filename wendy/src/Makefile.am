bin_PROGRAMS = wendy

wendy_SOURCES = syntax_graph.yy lexic_graph.ll \
                main.cc \
                FullMarking.h FullMarking.cc \
                InterfaceMarking.h InterfaceMarking.cc \
                Label.h Label.cc \
                InnerMarking.h InnerMarking.cc \
                Knowledge.h Knowledge.cc \
		StoredKnowledge.h StoredKnowledge.cc

wendy_LDADD =  $(top_builddir)/libs/libpnapi.a

wendy_CPPFLAGS = -I$(top_srcdir)/libs

EXTRA_DIST = syntax_graph.h

MAINTAINERCLEANFILES = syntax_graph.h

doxygen:
	cd ../doc ; doxygen
