.SUFFIXES = .ggo

noinst_PROGRAMS =  graph2dot form

AM_CPPFLAGS = -I$(top_srcdir)

graph2dot_SOURCES = syntax_graph.yy lexic_graph.ll \
                    cmdline-graph2dot.c cmdline-graph2dot.h \
                    graph2dot.cc

form_SOURCES = form-syntax.yy form-lexic.ll

#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
BUILT_SOURCES =
EXTRA_DIST =
MAINTAINERCLEANFILES =
DISTCLEANFILES =

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(graph2dot_SOURCES)) $(filter %.yy,$(form_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
BUILT_SOURCES += $(BISON_FILES:.yy=.hh)
EXTRA_DIST += $(BISON_FILES:.yy=.hh)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.hh)

# fix for Automake 1.11
$(BISON_FILES:.yy=.hh):
	$(AM_V_GEN) mv $(@:.hh=.h) $@

#----------------------------------------------------------#
# GENERIC: use GNU gengetopt to create command line parser #
#----------------------------------------------------------#
.ggo.c:
	$(AM_V_GEN)$(GENGETOPT) --input=$< -F $(@:.c=)
GENGETOPT_FILES = cmdline-graph2dot.ggo
BUILT_SOURCES += $(GENGETOPT_FILES:.ggo=.c)
EXTRA_DIST += $(GENGETOPT_FILES)
MAINTAINERCLEANFILES += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
