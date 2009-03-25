bin_PROGRAMS = petri
noinst_PROGRAMS = generator

petri_SOURCES = petri.cc cmdline.c cmdline.h
generator_SOURCES = generator.cc

petri_LDADD =  $(top_builddir)/src/libpnapi.a
generator_LDADD =  $(top_builddir)/src/libpnapi.a

BUILT_SOURCES = cmdline.c

AM_CPPFLAGS = -I$(top_srcdir)/src

EXTRA_DIST = cmdline.ggo generator.cc

DISTCLEANFILES = cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=$(top_srcdir)/utils/cmdline.ggo
