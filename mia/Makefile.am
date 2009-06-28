# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src man tests doc maintainer
else
SUBDIRS = src man tests doc maintainer
endif

EXTRA_DIST = REQUIREMENTS

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in \
	       src/mia.conf
	rm -fr man/Makefile.in
	rm -fr doc/doxygen doc/mdate-sh
	rm -fr libs/pnapi/Makefile.in
	rm -f tests/Makefile.in
	rm -f maintainer/Makefile.in
	rm -f doc/Makefile.in

