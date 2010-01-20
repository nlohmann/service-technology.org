SUBDIRS = src/productOG src/minimize tests

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
	-rm -f src/productOG/Makefile.in src/minimize/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/config.h.in
	-rm -f tests/Makefile.in
	-rm -f man/Makefile.in
	-rm -f maintainer/Makefile.in
	-rm -f INSTALL
	@echo -----------------------------------------------------
	@echo To recreate installation status enter
	@echo "  autoreconf -i"
	@echo

