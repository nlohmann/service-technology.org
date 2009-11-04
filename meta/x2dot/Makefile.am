SUBDIRS = src/og2dot 

# delete all files that can be created by other files
svn-clean: maintainer-clean
	-rm -f compile
	-rm -f aclocal.m4
	-rm -f configure
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f ylwrap
	-rm -f -r autom4te.cache
	-rm -f src/og2dot/Makefile.in
	-rm -f -r src/og2dot/.deps
	-rm -f src/og2dot/config.h.in
	-rm -f tests/Makefile.in
	-rm -fr doc/doxygen
	-rm -f doc/Doxyfile doc/mdate-sh
	-rm -f doc/Makefile.in
	-rm -f man/Makefile.in
	-rm -f maintainer/Makefile.in
	-rm -f INSTALL
	@echo -----------------------------------------------------
	@echo To recreate installation status enter
	@echo "  autoreconf -i"
	@echo

doxygen:
	cd doc ; doxygen
