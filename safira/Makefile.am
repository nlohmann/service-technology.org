SUBDIRS = src minisat tests man doc maintainer

#############################################################################
# remove everything that cannot be built
#############################################################################
svn-clean: maintainer-clean
	rm -fr configure Makefile.in depcomp compile INSTALL missing aclocal.m4 install-sh ylwrap tests/Makefile.in src/Makefile.in src/config.h.in man/Makefile.in minisat/Makefile.in doc/Makefile.in doc/mdate-sh
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

