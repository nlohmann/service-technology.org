bin_PROGRAMS = og2dot

og2dot_SOURCES = og-syntax.yy og-lexic.ll \
		 main.cc config.h \
                 cmdline.c cmdline.h 

BUILT_SOURCES = cmdline.c og-syntax.cc

EXTRA_DIST = cmdline.ggo og-syntax.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h og-syntax.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
