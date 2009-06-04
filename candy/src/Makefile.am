# RICH Datei von linda
bin_PROGRAMS = candy

candy_SOURCES = og-syntax.yy og-lexic.ll \
                Graph.cc Graph.h Node.cc Node.h \
                main.cc config.h \
                cmdline.c cmdline.h

#linda_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a $(top_builddir)/libs/lp_solve/liblpsolve55.a
candy_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a

#linda_CPPFLAGS = -I$(top_srcdir)/libs/pnapi -I$(top_srcdir)/libs/lp_solve
candy_CPPFLAGS = -I$(top_srcdir)/libs/pnapi

BUILT_SOURCES = cmdline.c og-syntax.cc

EXTRA_DIST = cmdline.ggo og-syntax.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h og-syntax.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
