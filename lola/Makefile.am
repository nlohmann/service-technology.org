SUBDIRS = src doc tests

EXTRA_DIST = patches

##############################################################################
# remove everything that can be rebuilt
##############################################################################
svn-clean: maintainer-clean
	-rm -f aclocal.m4 COPYING INSTALL depcomp configure Makefile.in \
	  missing ylwrap install-sh config.guess config.sub \
	  src/readnet-syntax.h src/Makefile.in src/config.H.in \
	  tests/Makefile.in doc/mdate-sh doc/Makefile.in src/.deps
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  aclocal"
	@echo "  autoconf"
	@echo "  autoheader"
	@echo "  automake -ac"
	@echo
	@echo "or"
	@echo "  autoreconf -i"
	@echo
