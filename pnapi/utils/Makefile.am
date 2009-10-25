bin_PROGRAMS = petri sa2owfn sa2sm
noinst_PROGRAMS = generator

petri_SOURCES = petri.cc cmdline.c cmdline.h
generator_SOURCES = generator.cc cmdline.ggo
sa2owfn_SOURCES = sa2owfn.cc
sa2sm_SOURCES = sa2sm.cc

petri_LDADD =  $(top_builddir)/src/libpnapi.la
generator_LDADD =  $(top_builddir)/src/libpnapi.la
sa2owfn_LDADD =  $(top_builddir)/src/libpnapi.la
sa2sm_LDADD =  $(top_builddir)/src/libpnapi.la

BUILT_SOURCES = cmdline.c

EXTRA_DIST = cmdline.ggo

# get flags from configure script
AM_LDFLAGS  = @configured_LDFLAGS@
AM_CXXFLAGS = @configured_CXXFLAGS@
AM_CFLAGS = @configured_CXXFLAGS@ 

AM_CPPFLAGS = -I$(top_srcdir)/src

DISTCLEANFILES = cmdline.c cmdline.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(AM_V_GEN)$(GENGETOPT) --input=$(srcdir)/cmdline.ggo --set-version=$(VERSION)

