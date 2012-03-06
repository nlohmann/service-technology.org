bin_PROGRAMS = pruner

pruner_SOURCES = pruner.cc \
                 pruner-statespace.h pruner-statespace.cc \
		 Output.h Output.cc \
		 pruner_lexer_graph.ll pruner_parser_graph.yy \
		 pruner_lexer_communication.ll pruner_parser_communication.yy \
                 pruner-cmdline.c pruner-cmdline.h \
                 pruner-helper.h pruner-helper.cc \
		 pruner-verbose.h pruner-verbose.cc

pruner_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\" -I$(top_srcdir)/src

AM_CPPFLAGS = -I$(top_srcdir)/src

# only add the local Petri net API if necessary
if COMPILE_PNAPI
pruner_CPPFLAGS += -I$(top_srcdir)/libs
pruner_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif

GGOFILES = pruner-cmdline.ggo
BUILT_SOURCES = $(GGOFILES:.ggo=.c)
EXTRA_DIST = $(GGOFILES)
DISTCLEANFILES = $(GGOFILES:.ggo=.c) $(GGOFILES:.ggo=.h)

################################################################
# use GNU gengetopt to create command line parser
################################################################
%.c: %.ggo
	$(AM_V_GEN)$(GENGETOPT) --input=$(srcdir)/$< --set-version=$(VERSION) --file-name=$(@:.c=)


#---------------------------------------------------#
# GENERIC: initialization for other generic targets #
#---------------------------------------------------#
MAINTAINERCLEANFILES =


#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(pruner_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.h)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.h)

