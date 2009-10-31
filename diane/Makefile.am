SUBDIRS = libs/pnapi src man maintainer doc tests

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure config.guess config.sub config.log \
         depcomp install-sh missing ylwrap man/Makefile.in tests/Makefile.in
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr doc/doxygen doc/Makefile.in doc/mdate-sh
	rm -fr libs/pnapi/Makefile.in maintainer/Makefile.in

doxygen:
	cd doc/ ; doxygen
