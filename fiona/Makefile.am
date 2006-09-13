############################################################################
# Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Dennis Reinert,   #
#                      Jan Bretschneider and Christian Gierds              #
#                                                                          #
# This file is part of Fiona.                                              #
#                                                                          #
# Fiona is free software; you can redistribute it and/or modify it         #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# Fiona is distributed in the hope that it will be useful, but WITHOUT     #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with Fiona; if not, write to the Free Software Foundation, Inc., 51      #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

# File last changed $Date: 2006-09-13 09:30:55 $ by $Author: bretschn $.
# Version $Revision: 1.5 $

SUBDIRS = cudd src tests

.PHONY: binarydistreadme

# all files that require the permission to execute
SHELLSCRIPTS = tests/testSequenceSuite tests/testSequenceSuite2

# delete all files that can be created by other files
hu-prepare: maintainer-clean
	-rm -f COPYING
	-rm -f aclocal.m4
	-rm -f configure
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f -r autom4te.cache
	-rm -f src/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/Fiona.h.in
	-rm -f tests/Makefile.in
	find -type d -exec chmod 755 {} \;
	find -type f -exec chmod 644 {} \;
	@echo -----------------------------------------------------
	@echo To recreate installation status enter
	@echo   aclocal
	@echo   autoconf
	@echo   autoheader
	@echo   automake -a -c
	@echo

# a README file for a binary distribution
binarydistreadme:
	@echo -e Copyright \(C\) 2005, 2006 Peter Massuthe and Daniela Weinberg > BINARYREADME
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
win32:
	(cd cudd && $(MAKE) win32)
	(cd src && $(MAKE) CXXFLAGS="$(CXXFLAGS) -mno-cygwin -U _WIN32" CFLAGS="$(CFLAGS) -mno-cygwin -U _WIN32")


# name of the directory/archive for a binary distribution
BINARYDISTDIR = $(distdir)

# files that are distributed in a binary distribution
BINARYDISTFILES = src/$(PACKAGE)$(EXEEXT) COPYING

BINARYFILES = src/$(PACKAGE)$(EXEEXT) COPYING

# target to create a binary distribution
bindist: clean binarydistreadme all create_bindist_archive

# target to create a binary distribution for systems having no cygwin1.dll.
win32bindist: clean binarydistreadme win32
	$(MAKE) create_bindist_archive BINARYDISTDIR="$(distdir)-win32"

# Helper target for bindist and win32bindist. Creates the actual zip file to be
# distributed.
create_bindist_archive:
	$(mkdir_p) $(BINARYDISTDIR)
	cp $(BINARYFILES) $(BINARYDISTDIR)
	mv BINARYREADME $(BINARYDISTDIR)/README
	zip -9 -m -r $(BINARYDISTDIR).zip $(BINARYDISTDIR)


clean:
	for subdir in $(SUBDIRS); do \
		if [ "$$subdir" != "cudd" ]; then \
			(cd "$$subdir" && $(MAKE) $(AM_MAKEFLAGS) $@); \
		fi \
	done

libclean: clean
	(cd cudd && $(MAKE) $(AM_MAKEFLAGS) clean)
