# RICH Datei von linda
bin_PROGRAMS = candy

candy_SOURCES = main.cc \
        helpers.h helpers.cc \
        bounds.h  \
        messageprofile.h  \
    	cmdline.c cmdline.h

#linda_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a $(top_builddir)/libs/lp_solve/liblpsolve55.a
candy_LDADD =  $(top_builddir)/libs/pnapi/libpnapi.a

#linda_CPPFLAGS = -I$(top_srcdir)/libs/pnapi -I$(top_srcdir)/libs/lp_solve
candy_CPPFLAGS = -I$(top_srcdir)/libs/pnapi

BUILT_SOURCES = cmdline.c

EXTRA_DIST = cmdline.ggo

MAINTAINERCLEANFILES = cmdline.c cmdline.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
