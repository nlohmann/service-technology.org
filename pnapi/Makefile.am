SUBDIRS = src tests utils

# delete all files that can be created by other files
svn-clean: maintainer-clean
	-rm -f compile config.guess config.sub
	-rm -f aclocal.m4
	-rm -fr m4 ltmain.sh
	-rm -f configure Doxyfile compile
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f ylwrap
	-rm -f -r autom4te.cache
	-rm -f src/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/config.h.in
	-rm -f tests/Makefile.in
	-rm -f tests/petrinet/Makefile.in tests/automaton/Makefile.in 
	-rm -f utils/Makefile.in
	-rm -fr doc/doxygen
	-rm -f doc/Doxyfile
	-rm -f INSTALL
	@echo -----------------------------------------------------
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pnapi.pc
