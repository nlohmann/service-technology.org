bin_PROGRAMS = linda

linda_SOURCES = main.cc \
		setsOfFinalMarkings.cc setsOfFinalMarkings.h \
		stateEquation.cc stateEquation.h \
		eventTerm.cc eventTerm.h \
        helpers.h helpers.cc \
        bounds.h  \
        messageprofile.h  \
        cmdline.c cmdline.h \
        syntax_et.yy lexer_et.ll


linda_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.a
linda_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve

# only add the local Petri net API if necessary
if COMPILE_PNAPI
linda_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
linda_CPPFLAGS += -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c syntax_et.cc syntax_et.h lexer_et.cc

EXTRA_DIST = syntax_et.h cmdline.ggo

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_et.cc syntax_et.h lexer_et.cc


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
