SUBDIRS = libs utils/pccts src maintainer doc tests

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh \
	src/config.h.in \
        build-aux m4/ltsugar.m4 m4/libtool.m4 m4/ltversion.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 \
	src/genet.exe.stackdump \
	utils/pccts/antlr/Makefile.in utils/pccts/sorcerer/Makefile.in \
	utils/pccts/h/Makefile.in utils/pccts/dlg/Makefile.in \
	utils/pccts/support/genmk/Makefile.in libs/cudd/Makefile.in
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done	

