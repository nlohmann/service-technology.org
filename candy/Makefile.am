SUBDIRS = src tests doc man

svn-clean: maintainer-clean
	rm -fr config.guess config.sub COPYING INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap autom4te.cache
	rm -fr src/Makefile.in src/config.h.in~ src/config.h.in
	rm -fr tests/Makefile.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen doc/Doxyfile doc/mdate-sh doc/Makefile.in

doxygen:
	cd doc ; doxygen
