bin_PROGRAMS = diane 

diane_SOURCES =  main.cc \
                 unionfind.cc unionfind.h \
                 cmdline.c cmdline.h

# only add the local Petri net API if necessary
if COMPILE_PNAPI
diane_LDADD = $(top_builddir)/libs/pnapi/libpnapi.a
diane_CPPFLAGS = -I$(top_srcdir)/libs
endif

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
