SUBDIRS = libs/pnapi src tests

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	  depcomp install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub \
	  config.guess config.h.in~ libs/pnapi/Makefile.in
	rm -fr src/Makefile.in src/config.h.in src/config.h.in~ tests/Makefile.in
	rm -fr build-aux
