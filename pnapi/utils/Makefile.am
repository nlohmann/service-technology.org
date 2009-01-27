noinst_PROGRAMS = petri

petri_SOURCES = main.cc cmdline.c cmdline.h
BUILT_SOURCES = cmdline.c

petri_CPPFLAGS = -I$(top_srcdir)/src
petri_LDADD =  $(top_builddir)/src/libpnapi.a

EXTRA_DIST = cmdline.ggo
MAINTAINERCLEANFILES = cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
