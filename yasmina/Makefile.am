SUBDIRS = libs/pnapi libs/lp_solve src man doc tests 

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap config.guess ltmain.sh config.sub
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen doc/Doxyfile doc/mdate-sh
	rm -fr libs/lp_solve/Makefile.in libs/pnapi/Makefile.in
	rm -f -r src/.deps tests/Makefile.in doc/Makefile.in
	rm -fr m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
	rm -fr build-aux

