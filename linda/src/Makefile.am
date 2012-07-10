bin_PROGRAMS = linda

linda_SOURCES = main.cc \
		stateEquation.cc stateEquation.h \
		eventTerm.cc eventTerm.h \
		helpers.h helpers.cc \
		cmdline.c cmdline.h \
		syntax_etc.yy lexer_etc.ll \
		syntax_et.yy lexer_et.ll \
		syntax_profile.yy lexer_profile.ll \
		syntax_request.yy lexer_request.ll \
		verbose.h verbose.cc \
		Output.h Output.cc \
		lindaAgent.h lindaAgent.cc \
		eventTermParser.h eventTermParser.cc \
		staticAnalysis.h staticAnalysis.cc \
		costs.cc costs.h

linda_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
linda_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve -I$(top_srcdir)/libs

# only add the local Petri net API if necessary
if COMPILE_PNAPI
linda_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
linda_CPPFLAGS += -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c 

EXTRA_DIST = cmdline.ggo

MAINTAINERCLEANFILES = cmdline.c cmdline.h

#--------------------------------------------------------------#
# GENERIC: organize distribution and cleaning of Bison sources #
#--------------------------------------------------------------#
BISON_FILES = $(filter %.yy,$(linda_SOURCES))
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
