# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src utils man doc maintainer tests
else
SUBDIRS = src utils man doc maintainer tests
endif

EXTRA_DIST = REQUIREMENTS

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh ylwrap src/Makefile.in \
	src/config.h.in maintainer/Makefile.in libs/pnapi/Makefile.in \
	utils/Makefile.in tests/Makefile.in doc/Makefile.in doc/mdate-sh \
	man/Makefile.in src/config-log.h
