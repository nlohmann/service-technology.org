# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src man doc tests
else
SUBDIRS = src man doc tests
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in INSTALL aclocal.m4 src/Makefile.in \
		src/config.h.in libs/pnapi/Makefile.in \
		tests/Makefile.in doc/Makefile.in doc/mdate-sh \
		man/Makefile.in src/config-log.h build-aux
