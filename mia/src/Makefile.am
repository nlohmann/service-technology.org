bin_PROGRAMS = mia

mia_SOURCES = syntax_graph.yy lexic_graph.ll \
              syntax_im.yy lexic_im.ll \
              main.cc \
              cmdline.c cmdline.h


# only add the local Petri net API if necessary
if COMPILE_PNAPI
mia_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
mia_CPPFLAGS = -I$(top_srcdir)/libs
endif

BUILT_SOURCES = cmdline.c syntax_graph.cc syntax_im.cc
EXTRA_DIST = syntax_graph.h syntax_im.h cmdline.ggo
MAINTAINERCLEANFILES = syntax_graph.h syntax_im.h cmdline.c cmdline.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
