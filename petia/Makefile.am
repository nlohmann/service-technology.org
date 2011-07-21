# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install
       
# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src man doc tests
else
SUBDIRS = libs/lp_solve src man doc tests
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess config.sub \
	INSTALL missing aclocal.m4 install-sh ylwrap \
	src/config.h.in doc/mdate-sh src/config-log.h build-aux
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done
