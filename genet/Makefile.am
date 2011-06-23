SUBDIRS = libs utils/pccts src maintainer

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh ylwrap src/Makefile.in \
	src/config.h.in maintainer/Makefile.in \
	libs/Makefile.in libs/cudd/Makefile.in libs/alglib/Makefile.in

