.SUFFIXES = .ggo

noinst_PROGRAMS =  graph2dot path2graph

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = @configured_CFLAGS@
AM_CXXFLAGS = @configured_CXXFLAGS@
AM_LDFLAGS = @configured_LDFLAGS@

graph2dot_SOURCES = cmdline-graph2dot.c cmdline-graph2dot.h \
                    syntax_graph.yy lexic_graph.ll \
                    graph2dot.cc

path2graph_SOURCES = cmdline-path2graph.c cmdline-path2graph.h \
                     syntax_net.yy syntax_path.yy \
                     lexic_net.ll lexic_path.ll \
                     path2graph.cc

EXTRA_DIST = syntax_graph.h syntax_net.h syntax_path.h \
             cmdline-graph2dot.ggo cmdline-path2graph.ggo

MAINTAINERCLEANFILES = syntax_graph.h syntax_net.h syntax_path.h \
                       cmdline-graph2dot.c cmdline-graph2dot.h \
                       cmdline-path2graph.c cmdline-path2graph.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
.ggo.c:
	$(GENGETOPT) --input=$< -F $(@:.c=)
