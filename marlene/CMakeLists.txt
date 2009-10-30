cmake_minimum_required(VERSION 2.6)

add_subdirectory(libs/pnapi)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)
add_subdirectory(man)
add_subdirectory(maintainer)

########### install files ###############




#original Makefile.am contents follow:

## process "libs/pnapi" directory only if the Petri Net API needs to be compiled
#if COMPILE_PNAPI
#SUBDIRS = libs/pnapi src tests doc man maintainer
#else
#SUBDIRS = src tests doc man maintainer
#endif
#
#svn-clean: maintainer-clean
#	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
#	       depcomp install-sh missing ylwrap config.guess config.sub
#	rm -fr src/config.h.in~ src/Makefile.in libs/Makefile.in src/config.h.in
#	rm -fr maintainer/Makefile.in
#	rm -fr man/Makefile.in libs/pnapi/Makefile.in
#	rm -fr doc/doxygen doc/Makefile.in doc/mdate-sh
#	rm -fr tests/Makefile.in
#
