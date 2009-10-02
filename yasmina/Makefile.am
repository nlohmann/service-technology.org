SUBDIRS = libs/pnapi libs/lp_solve src man

svn-clean: maintainer-clean
	rm -fr COPYING INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen
	rm -fr libs/lp_solve/Makefile.in libs/pnapi/Makefile.in

