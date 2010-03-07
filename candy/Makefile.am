# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = src doc tests

svn-clean: maintainer-clean
	rm -fr config.guess config.sub INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap autom4te.cache
	rm -fr src/Makefile.in src/config.h.in~ src/config.h.in src/config-log.h
	rm -fr tests/Makefile.in
	rm -fr doc/mdate-sh doc/Makefile.in
