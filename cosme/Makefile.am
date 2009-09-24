# process "libs" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src 
else
SUBDIRS = src
endif

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	  depcomp install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub \
	  config.guess config.h.in~ libs/pnapi/Makefile.in
	rm -fr src/Makefile.in src/config.h.in src/config.h.in~
