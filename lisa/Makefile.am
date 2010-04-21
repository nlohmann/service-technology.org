# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install
       
# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src man tests
else
SUBDIRS = libs/lp_solve src man maintainer tests
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh ylwrap src/Makefile.in \
	src/config.h.in maintainer/Makefile.in libs/pnapi/Makefile.in libs/lp_solve/Makefile.in \
	tests/Makefile.in doc/Makefile.in doc/mdate-sh \
	man/Makefile.in src/config-log.h build-aux \
	m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
