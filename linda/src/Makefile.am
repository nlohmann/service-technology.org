#lib_LTLIBRARIES = liblinda.la

#liblinda_la_SOURCES = main.cc \
#		setsOfFinalMarkings.cc setsOfFinalMarkings.h \
#		stateEquation.cc stateEquation.h \
#		eventTerm.cc eventTerm.h \
#		helpers.h helpers.cc \
#		cmdline.c cmdline.h \
#		syntax_etc.yy lexer_etc.ll \
#		syntax_et.yy lexer_et.ll \
#		files.h files.cc \
#		verbose.h verbose.cc \
#		lindaAgent.h lindaAgent.cc \
#		eventTermParser.h eventTermParser.cc 

#liblinda_la_LIBADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
#liblinda_la_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve

# information for libtool
#includedir = @includedir@/linda

#include_HEADERS = lindaAgent.h setsOfFinalMarkings.h stateEquation.h eventTerm.h helpers.h cmdline.h files.h verbose.h

# only add the local Petri net API if necessary
#if COMPILE_PNAPI
#liblinda_la_LIBADD += $(top_builddir)/libs/pnapi/libpnapi.a
#liblinda_la_CPPFLAGS += -I$(top_srcdir)/libs
#endif



bin_PROGRAMS = linda

linda_SOURCES = main.cc \
		setsOfFinalMarkings.cc setsOfFinalMarkings.h \
		stateEquation.cc stateEquation.h \
		eventTerm.cc eventTerm.h \
		helpers.h helpers.cc \
		cmdline.c cmdline.h \
		syntax_etc.yy lexer_etc.ll \
		syntax_et.yy lexer_et.ll \
		files.h files.cc \
		verbose.h verbose.cc \
		lindaAgent.h lindaAgent.cc \
		eventTermParser.h eventTermParser.cc 

linda_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
linda_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve

# only add the local Petri net API if necessary
if COMPILE_PNAPI
linda_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
linda_CPPFLAGS += -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c syntax_et.cc syntax_et.h lexer_et.cc lexer_etc.cc syntax_etc.cc syntax_etc.h 

EXTRA_DIST = syntax_et.h syntax_etc.h cmdline.ggo

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_et.cc syntax_et.h lexer_et.cc


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
