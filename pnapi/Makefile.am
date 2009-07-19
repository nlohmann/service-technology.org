SUBDIRS = src tests utils man

# delete all files that can be created by other files
svn-clean: maintainer-clean
	rm -fr m4 configure Makefile.in depcomp compile config.guess \
	ltmain.sh config.sub INSTALL missing aclocal.m4 install-sh ylwrap \
	man/Makefile.in utils/Makefile.in tests/Makefile.in \
	tests/petrinet/Makefile.in src/Makefile.in src/config.h.in
	@echo -----------------------------------------------------
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pnapi.pc
