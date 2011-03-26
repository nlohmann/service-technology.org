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

BUILT_SOURCES = cmdline.c syntax_et.cc syntax_et.h lexer_et.cc lexer_etc.cc syntax_etc.cc syntax_etc.h 

EXTRA_DIST = syntax_et.h syntax_etc.h cmdline.ggo syntax_profile.h syntax_request.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_et.cc syntax_et.h lexer_et.cc syntax_request.h syntax_profile.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
