bin_PROGRAMS = petri
noinst_PROGRAMS = generator

petri_SOURCES = petri.cc cmdline.c cmdline.h
generator_SOURCES = generator.cc cmdline.ggo

petri_LDADD =  $(top_builddir)/src/libpnapi.la
generator_LDADD =  $(top_builddir)/src/libpnapi.la

BUILT_SOURCES = cmdline.c

AM_CPPFLAGS = -I$(top_srcdir)/src

DISTCLEANFILES = cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --set-version=$(VERSION)
