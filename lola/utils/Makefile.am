.SUFFIXES = .ggo

bin_PROGRAMS =  graph2dot

AM_CPPFLAGS = -I$(top_srcdir)

graph2dot_SOURCES = cmdline-graph2dot.c cmdline-graph2dot.h \
                    syntax_graph.yy lexic_graph.ll \
                    graph2dot.cc

EXTRA_DIST = syntax_graph.h cmdline-graph2dot.ggo

MAINTAINERCLEANFILES = syntax_graph.h cmdline-graph2dot.c cmdline-graph2dot.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
.ggo.c:
	$(AM_V_GEN)$(GENGETOPT) --input=$< -F $(@:.c=)
