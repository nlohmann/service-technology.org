bin_PROGRAMS = sayo

sayo_SOURCES =  syntax_og.yy lexic_og.ll \
                main.cc config.h \
                cmdline.c cmdline.h 

BUILT_SOURCES = cmdline.c syntax_og.cc

EXTRA_DIST = cmdline.ggo syntax_og.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h syntax_og.h

################################################################
# inherit flags set by the configure script
################################################################

sayo_LDFLAGS  = @configured_LDFLAGS@
sayo_CFLAGS   = @configured_CFLAGS@
sayo_CXXFLAGS = @configured_CXXFLAGS@

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
