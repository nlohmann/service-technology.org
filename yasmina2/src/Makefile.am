bin_PROGRAMS = yasmina

yasmina_SOURCES = main.cc \
		helpers.h helpers.cc \
		cmdline.c cmdline.h \
		verbose.h verbose.cc \
		Output.h Output.cc \
		lexer_fingerprint.ll syntax_fingerprint.yy

yasmina_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
yasmina_CPPFLAGS = -I$(top_srcdir)/libs

# only add the local Petri net API if necessary
if COMPILE_PNAPI
yasmina_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
yasmina_CPPFLAGS += -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c

EXTRA_DIST = cmdline.ggo

MAINTAINERCLEANFILES = cmdline.c cmdline.h

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(yasmina_SOURCES))
BUILT_SOURCES += $(BISON_FILES:.yy=.cc)
EXTRA_DIST += $(BISON_FILES:.yy=.hh)
MAINTAINERCLEANFILES += $(BISON_FILES:.yy=.hh)

# fix for Automake 1.12
$(BISON_FILES:.yy=.hh):
	$(AM_V_GEN) mv $(@:.hh=.h) $@

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
