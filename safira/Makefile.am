SUBDIRS = libs/minisat src tests man doc

#############################################################################
# remove everything that cannot be built
#############################################################################
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL missing aclocal.m4 install-sh ylwrap tests/Makefile.in src/Makefile.in src/config.h.in man/Makefile.in doc/Makefile.in doc/mdate-sh libs/minisat/Makefile.in
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

