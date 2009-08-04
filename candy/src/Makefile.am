bin_PROGRAMS = candy

candy_SOURCES = og-syntax.yy og-lexic.ll \
                cf-syntax.yy cf-lexic.ll \
                nf-syntax.yy nf-lexic.ll \
                settings.h \
                Graph.cc Graph.h \
                Node.cc Node.h \
                Formula.cc Formula.h \
                Event.h \
                main.cc config.h \
                cmdline.c cmdline.h

BUILT_SOURCES = cmdline.c og-syntax.cc cf-syntax.cc nf-syntax.cc

EXTRA_DIST = cmdline.ggo og-syntax.h cf-syntax.h nf-syntax.h

MAINTAINERCLEANFILES = cmdline.c cmdline.h og-syntax.h cf-syntax.h nf-syntax.h


################################################################
# use GNU gengetopt to create command line parser
################################################################
cmdline.c: cmdline.ggo
	$(GENGETOPT) --input=cmdline.ggo


doxygen:
	cd ../doc ; doxygen
