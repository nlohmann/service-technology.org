bin_PROGRAMS = yasmina

yasmina_SOURCES = main.cc cmdline.c cmdline.h verbose.cc verbose.h \
		  syntax_mp.yy lexer_mp.ll eventTerm.h eventTerm.cc

yasmina_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a $(top_builddir)/libs/lp_solve/liblpsolve55.a

yasmina_LDFLAGS = -ldl

yasmina_CPPFLAGS = -I$(top_srcdir)/libs/pnapi -I$(top_srcdir)/libs/lp_solve

BUILT_SOURCES = cmdline.c syntax_mp.cc  

EXTRA_DIST = cmdline.ggo syntax_mp.h 

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_mp.h 


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
