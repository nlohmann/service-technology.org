bin_PROGRAMS = petri sa2owfn
noinst_PROGRAMS = generator

petri_SOURCES = petri.cc cmdline.c cmdline.h
generator_SOURCES = generator.cc cmdline.ggo
sa2owfn_SOURCES = sa2owfn.cc

petri_LDADD =  $(top_builddir)/src/libpnapi.la
generator_LDADD =  $(top_builddir)/src/libpnapi.la
sa2owfn_LDADD =  $(top_builddir)/src/libpnapi.la

BUILT_SOURCES = cmdline.c

AM_CPPFLAGS = -I$(top_srcdir)/src

DISTCLEANFILES = cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo --set-version=$(VERSION)
