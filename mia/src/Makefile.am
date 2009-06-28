bin_PROGRAMS = mia

mia_SOURCES = syntax_graph.yy lexic_graph.ll \
              syntax_im.yy lexic_im.ll \
              syntax_mi.yy lexic_mi.ll \
              main.cc \
              verbose.cc verbose.h \
              cmdline.c cmdline.h

sysconf_DATA = mia.conf

# pass the sysconfdir to Mia
mia_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"

# only add the local Petri net API if necessary
if COMPILE_PNAPI
mia_CPPFLAGS += -I$(top_srcdir)/libs
mia_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif

BUILT_SOURCES = cmdline.c syntax_graph.cc syntax_im.cc syntax_mi.cc
EXTRA_DIST = syntax_graph.h syntax_im.h syntax_mi.h cmdline.ggo mia.conf
MAINTAINERCLEANFILES = syntax_graph.h syntax_im.h syntax_mi.h cmdline.c cmdline.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
