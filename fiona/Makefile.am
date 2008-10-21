SUBDIRS = libs src tests 
EXTRA_DIST = doc/docShop.owfn doc/docShop.owfn.og doc/fionaDocumentation.pdf


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
	-rm -f -r autom4te.cache
	-rm -f src/Makefile.in
	-rm -f libs/Makefile.in libs/cudd/Makefile.in libs/pnapi/Makefile.in
	-rm -f man/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/Fiona.h.in
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

.PHONY: binarydistreadme

# a README file for a binary distribution
binarydistreadme:
	@echo -e Copyright \(C\) 2005, 2006, 2007, 2008 Peter Massuthe and Daniela Weinberg > BINARYREADME
	@echo >> BINARYREADME
	@echo -e This is a binary distribution of Fiona, a tool translating business >> BINARYREADME
	@echo -e processes described in BPEL to open workflow nets \(oWFN\). The sources >> BINARYREADME
	@echo -e of Fiona can be downloaded from the website of Fiona >> BINARYREADME
	@echo -e \(http://www.informatik.hu-berlin.de/top/tools4bpel/Fiona\). >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e Fiona is free software\; you can redistribute it and/or modify it >> BINARYREADME
	@echo -e under the terms of the GNU General Public License as published by the >> BINARYREADME
	@echo -e Free Software Foundation\; either version 2 of the License, or \(at your >> BINARYREADME
	@echo -e option\) any later version. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e Fiona is distributed in the hope that it will be useful, but >> BINARYREADME
	@echo -e WITHOUT ANY WARRANTY\; without even the implied warranty of >> BINARYREADME
	@echo -e MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU >> BINARYREADME
	@echo -e General Public License for more details. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e You should have received a copy of the GNU General Public License along >> BINARYREADME
	@echo -e with Fiona\; if not, write to the Free Software Foundation, Inc., 51 >> BINARYREADME
	@echo -e Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. >> BINARYREADME

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
