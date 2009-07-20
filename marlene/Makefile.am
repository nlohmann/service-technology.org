# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src tests
else
SUBDIRS = src tests
endif

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen doc/Makefile.in doc/mdate-sh
	rm -fr tests/Makefile.in
