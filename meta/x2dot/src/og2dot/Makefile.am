bin_PROGRAMS = og2dot

og2dot_SOURCES = og-syntax.yy og-lexic.ll \
		 og_old-syntax.yy og_old-lexic.ll \
		 main.cc config.h \
                 cmdline.c cmdline.h 

BUILT_SOURCES = cmdline.c og-syntax.cc og_old-syntax.cc

EXTRA_DIST = cmdline.ggo og-syntax.h og_old-syntax.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h og-syntax.h og_old-syntax.h

################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
