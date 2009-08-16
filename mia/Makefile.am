# process "libs/pnapi" directory only if the Petri Net API needs to be compiled
if COMPILE_PNAPI
SUBDIRS = libs/pnapi src man tests doc maintainer
else
SUBDIRS = src man tests doc maintainer
endif

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL missing \
	aclocal.m4 install-sh ylwrap src/Makefile.in src/config.h.in \
	maintainer/Makefile.in libs/pnapi/Makefile.in tests/Makefile.in \
	doc/Makefile.in doc/mdate-sh man/Makefile.in src/config-log.h
