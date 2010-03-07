SUBDIRS = libs/pnapi src man tests doc

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL missing \
	aclocal.m4 install-sh ylwrap src/Makefile.in src/config.h.in \
	libs/pnapi/Makefile.in tests/Makefile.in \
	doc/Makefile.in doc/mdate-sh man/Makefile.in src/config-log.h
