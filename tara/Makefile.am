# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install
       
# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src doc tests
else
SUBDIRS = libs/lp_solve src doc tests 
endif

svn-clean: maintainer-clean
	rm -fr configure INSTALL aclocal.m4 src/config.h.in doc/mdate-sh src/config-log.h build-aux
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done

