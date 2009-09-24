bin_PROGRAMS = cosme

cosme_SOURCES = og-syntax.yy og-lexic.ll \
							graph-syntax.yy graph-lexic.ll \
							BitSetC.cc \
							Label.cc \
						  OperatingGuideline.cc Service.cc \
						  RBTreeStack.cc \
							main.cc \
							cmdline.c cmdline.h

EXTRA_DIST = og-syntax.h

MAINTAINERCLEANFILES = og-syntax.cc og-syntax.h og-lexic.cc

# only add the local Petri net API if necessary
if COMPILE_PNAPI
cosme_CPPFLAGS = -I$(top_srcdir)/libs
cosme_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
endif

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo
