SUBDIRS = src tests utils doc man

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# delete all files that can be created by other files
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile config.guess build-aux \
	ltmain.sh config.sub INSTALL missing aclocal.m4 install-sh ylwrap \
	man/Makefile.in utils/Makefile.in tests/Makefile.in \
	doc/Makefile.in doc/mdate-sh \
	tests/petrinet/Makefile.in src/Makefile.in src/config.h.in utils/unfolder/Makefile.in \
	m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
	@echo -----------------------------------------------------
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo
