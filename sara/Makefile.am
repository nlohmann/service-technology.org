# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src
else
SUBDIRS = libs/lp_solve src
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in INSTALL aclocal.m4 src/Makefile.in \
		src/config.h.in libs/pnapi/Makefile.in libs/lp_solve/Makefile.in \
		tests/Makefile.in doc/Makefile.in doc/mdate-sh src/config-log.h build-aux
