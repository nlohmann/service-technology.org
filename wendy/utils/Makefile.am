.SUFFIXES = .ggo

bin_PROGRAMS =  silence

AM_CPPFLAGS = -I$(top_srcdir)

silence_SOURCES = syntax_graph.yy lexic_graph.ll \
                    cmdline.c cmdline.h \
                    silence.cc Graph.cc Graph.h util.h verbose.cc verbose.h \
                    Output.cc Output.h


# only add the local Petri net API if necessary
if COMPILE_PNAPI
silence_CPPFLAGS = -I$(top_srcdir)/libs -I$(top_srcdir)/libs/pnapi
silence_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif

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
BISON_FILES = $(filter %.yy,$(silence_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)

#----------------------------------------------------------#
# GENERIC: use GNU gengetopt to create command line parser #
#----------------------------------------------------------#
.ggo.c:
	$(AM_V_GEN)$(GENGETOPT) --input=$< -F $(@:.c=)
GENGETOPT_FILES = cmdline.ggo
BUILT_SOURCES += $(GENGETOPT_FILES:.ggo=.c)
EXTRA_DIST += $(GENGETOPT_FILES)
MAINTAINERCLEANFILES += $(GENGETOPT_FILES:.ggo=.c) $(GENGETOPT_FILES:.ggo=.h)
