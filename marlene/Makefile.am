# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src tests doc man
else
SUBDIRS = src tests doc man
endif

svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	       depcomp install-sh missing ylwrap config.guess config.sub
	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
	rm -fr man/Makefile.in libs/pnapi/Makefile.in
	rm -fr doc/doxygen doc/Makefile.in doc/mdate-sh
	rm -fr tests/Makefile.in
	rm -fr build-aux

EXTRA_DIST = CMakeLists.txt man/CMakeLists.txt \
	     cmake/Bison.cmake cmake/Flex.cmake cmake/Gengetopt.cmake \
	     vs_header/stdint.h

