# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi libs/lp_solve src tests man doc
else
SUBDIRS = libs/lp_solve src tests man doc
endif

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in
	rm -fr doc/doxygen
	rm -fr libs/lp_solve/Makefile.in libs/pnapi/Makefile.in
	rm -fr tests/Makefile.in doc/Makefile.in doc/mdate-sh

