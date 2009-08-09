SUBDIRS = src tests minisat

#############################################################################
# remove everything that cannot be built
#############################################################################
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL COPYING missing aclocal.m4 install-sh ylwrap tests/Makefile.in src/Makefile.in src/config.h.in minisat/Makefile.in
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo
