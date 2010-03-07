# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src tests man doc
else
SUBDIRS = libs/lp_solve src tests man doc
endif

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap config.guess ltmain.sh config.sub
	rm -fr m4/libtool.4 lt~obsolete.m4 ltsugar.m4 ltversion.4
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen
	rm -fr libs/lp_solve/Makefile.in libs/pnapi/Makefile.in
	rm -fr tests/Makefile.in doc/Makefile.in doc/mdate-sh
	rm -fr build-aux

