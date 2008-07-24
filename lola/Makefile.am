SUBDIRS = src tests


EXTRA_DIST = doc

##############################################################################
# remove everything that can be rebuilt
##############################################################################
cvs-clean: maintainer-clean
	-rm -f aclocal.m4 COPYING INSTALL depcomp configure Makefile.in \
	  missing ylwrap install-sh config.guess config.sub \
	  src/readnet-syntax.h src/Makefile.in src/config.H.in \
	  tests/Makefile.in
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
