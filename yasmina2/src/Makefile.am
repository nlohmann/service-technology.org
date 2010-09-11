bin_PROGRAMS = yasmina

yasmina_SOURCES = main.cc \
		helpers.h helpers.cc \
		cmdline.c cmdline.h \
		verbose.h verbose.cc \
		Output.h Output.cc \
		lexer_fingerprint.ll syntax_fingerprint.yy
	
yasmina_LDADD = $(top_builddir)/libs/lp_solve/liblpsolve55.la
yasmina_CPPFLAGS = -I$(top_srcdir)/libs/lp_solve

# only add the local Petri net API if necessary
if COMPILE_PNAPI
yasmina_LDADD += $(top_builddir)/libs/pnapi/libpnapi.a
yasmina_CPPFLAGS += -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c lexer_fingerprint.cc syntax_fingerprint.cc syntax_fingerprint.h

EXTRA_DIST = cmdline.ggo syntax_fingerprint.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_fingerprint.cc syntax_fingerprint.h lexer_fingerprint.cc

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
