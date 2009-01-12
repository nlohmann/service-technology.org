SUBDIRS = libs src tests man doc


#############################################################################
# organize cleaning
#############################################################################

# delete all files that can be created by other files
svn-clean: maintainer-clean
	-rm -f COPYING Doxyfile config.guess config.sub INSTALL
	-rm -f aclocal.m4 compile ylwrap
	-rm -f configure
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f mdate-sh
	-rm -f -r autom4te.cache
	-rm -f src/Makefile.in
	-rm -f libs/Makefile.in libs/cudd/Makefile.in libs/pnapi/Makefile.in
	-rm -f man/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/config.h.in src/config.h.in~
	-rm -f tests/Makefile.in
	@echo -----------------------------------------------------
	@echo "To recreate installation status enter"
	@echo "  aclocal ; autoconf; autoheader; automake -a -c"
	@echo "or"
	@echo "  autoreconf -i"
	@echo


veryclean: clean
	$(MAKE) $(AM_MAKEFLAGS) veryclean --directory=src


#############################################################################
# organize binary distributions
#############################################################################

# a README file for a binary distribution
binarydistreadme: README
	cp README BINARYREADME

# target to create an executable that is independet of Cygwin
win32: clean
	$(top_srcdir)/configure --enable-win32
	$(MAKE) $(AM_MAKEFLAGS)

# name of the directory/archive for a binary distribution
BINARYDISTDIR = $(distdir)

# files that are distributed in a binary distribution
BINARYDISTFILES = src/$(PACKAGE)$(EXEEXT) COPYING

BINARYFILES = src/$(PACKAGE)$(EXEEXT) COPYING

bindist_generic:
	$(MAKE) $(AM_MAKEFLAGS) CXXFLAGS="-O3 -DCOMPILE_DATE=\"\\\"`date`\\\"\"" CFLAGS="-O3 -DCOMPILE_DATE=\"`date`\""

# target to create a binary distribution
bindist: clean binarydistreadme bindist_generic create_bindist_archive

# target to create a binary distribution for systems having no cygwin1.dll.
bindist_win32:
	$(top_srcdir)/configure --enable-win32 --disable-assert
	$(MAKE) bindist

bindist_universal:
	$(top_srcdir)/configure --enable-universal --disable-assert
	$(MAKE) bindist

# Helper target for bindist and win32bindist. Creates the actual zip file to be
# distributed.
create_bindist_archive:
	$(mkdir_p) $(BINARYDISTDIR)
	cp $(BINARYFILES) $(BINARYDISTDIR)
	mv BINARYREADME $(BINARYDISTDIR)/README
	zip -9 -m -r $(BINARYDISTDIR).zip $(BINARYDISTDIR)


#############################################################################
# organize checking (make check)
#############################################################################

# Do single checks in tests directory.
check-%: all
	(cd tests && $(MAKE) $(AM_MAKEFLAGS) $@)

# Do global memcheck in tests directory.
memcheck: all
	(cd tests && $(MAKE) $(AM_MAKEFLAGS) $@)

# Do single memchecks in tests directory.
memcheck-%: all
	(cd tests && $(MAKE) $(AM_MAKEFLAGS) $@)


#############################################################################
# organize checking code coverage
#############################################################################

# Do code coverage check for test suite. TODO: Check if it still works
allcov:
	(cd cudd  && $(MAKE) $(AM_MAKEFLAGS) all)
	(cd src   && $(MAKE) $(AM_MAKEFLAGS) allcov)

# Do code coverage check for test suite.
covcheck: allcov
	(cd tests && $(MAKE) $(AM_MAKEFLAGS) covcheck)


#########################################################################
# targets to organize creating and applying patches
#########################################################################
patch:
	@svn diff > $(PACKAGE)-rev-`svn info | $(GREP) "Last Changed Rev" | $(AWK) '{print $$4}'`.patch
	@echo "created patch $(PACKAGE)-rev-`svn info | $(GREP) "Last Changed Rev" | $(AWK) '{print $$4}'`.patch"
	@echo
	@echo "WARNING: This patch will only work for revision `svn info | $(GREP) "Last Changed Rev" | $(AWK) '{print $$4}'`."
	@echo "         Do not try to apply this patch to a different version"
	@echo "         unless you know what you are doing. You have been warned."
	@echo

patched: *.patch
	@echo
	@echo "WARNING: You are about to apply a patch to revision `svn info | $(GREP) "Last Changed Rev" | $(AWK) '{print $$4}'`".
	@echo "         Make sure the patch $< was made for this revision."
	@echo "         If you want to abort, press Ctrl+C now!"
	@echo "         Otherwise, press return to continue. You have been warned."
	@read
	@patch -p0 -i $< && echo "applied patch $<"
