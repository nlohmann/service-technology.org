SUBDIRS = libs src man doc maintainer tests

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap config.guess config.sub
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in maintainer/Makefile.in
	rm -fr doc/doxygen doc/Makefile.in doc/mdate-sh
	rm -fr tests/Makefile.in
