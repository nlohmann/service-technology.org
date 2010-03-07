# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install
       
# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src man doc maintainer tests
else
SUBDIRS = src man doc maintainer tests
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh ylwrap src/Makefile.in \
	src/config.h.in maintainer/Makefile.in libs/pnapi/Makefile.in \
	tests/Makefile.in doc/Makefile.in doc/mdate-sh \
	man/Makefile.in src/config-log.h
