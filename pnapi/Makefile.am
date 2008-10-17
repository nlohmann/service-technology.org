SUBDIRS = src tests

# delete all files that can be created by other files
svn-clean: maintainer-clean
	-rm -f COPYING
	-rm -f aclocal.m4
	-rm -f configure Doxyfile
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f -r autom4te.cache
	-rm -f src/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/config.h.in src/config.h.in~
	-rm -f tests/Makefile.in
	-rm -f INSTALL
	@echo -----------------------------------------------------
	@echo To recreate installation status enter
	@echo   aclocal
	@echo   autoconf
	@echo   autoheader
	@echo   automake -a -c
	@echo
