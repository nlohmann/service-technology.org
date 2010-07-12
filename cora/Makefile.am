# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src tests
else
SUBDIRS = src tests
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in INSTALL aclocal.m4 src/Makefile.in \
		src/config.h.in libs/pnapi/Makefile.in \
		tests/Makefile.in src/config-log.h build-aux \
		depcomp compile config.guess ltmain.sh config.sub missing install-sh \
		m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
