SUBDIRS = src man doc tests exp

# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

#############################################################################
# remove everything that cannot be built
#############################################################################
svn-clean: maintainer-clean
	rm -fr INSTALL Makefile.in aclocal.m4 compile configure \
	depcomp install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub \
	config.guess config.h.in~
	rm -fr doc/Makefile.in doc/mdate-sh doc/manual
	rm -fr man/Makefile.in
	rm -fr src/Makefile.in src/config.h.in src/config.h.in~ src/doc src/.deps
	rm -fr tests/Makefile.in
	rm -fr exp/Makefile.in exp/bpm2008
	rm -fr build-aux
	@echo "-----------------------------------------------------"
	@echo "To recreate installation status enter"
	@echo "  autoreconf -i"
	@echo

#############################################################################
# run experiments
#############################################################################
experiments:
	$(MAKE) $(AM_MAKEFLAGS) experiments -C exp
