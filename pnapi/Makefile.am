SUBDIRS = src tests utils utils/unfolder doc man

# delete all files that can be created by other files
svn-clean: maintainer-clean
	rm -fr m4 configure Makefile.in depcomp compile config.guess \
	ltmain.sh config.sub INSTALL missing aclocal.m4 install-sh ylwrap \
	man/Makefile.in utils/Makefile.in tests/Makefile.in \
	doc/Makefile.in doc/mdate-sh \
	tests/petrinet/Makefile.in src/Makefile.in src/config.h.in utils/unfolder/Makefile.in
	@echo -----------------------------------------------------
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pnapi.pc

# target to create an executable that is independet of Cygwin
win32: clean
	$(top_srcdir)/configure --enable-win32
	$(MAKE) $(AM_MAKEFLAGS)
