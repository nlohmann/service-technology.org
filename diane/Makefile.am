# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src maintainer
else
SUBDIRS = src maintainer
endif

svn-clean: maintainer-clean
	rm -fr COPYING INSTALL Makefile.in aclocal.m4 compile configure \
         depcomp install-sh missing ylwrap
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr doc/doxygen
	rm -fr libs/pnapi/Makefile.in maintainer/Makefile.in

doxygen:
	cd doc/ ; doxygen
