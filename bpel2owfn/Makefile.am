############################################################################
# Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert     #
#                                                                          #
# This file is part of BPEL2oWFN.                                          #
#                                                                          #
# BPEL2oWFN is free software; you can redistribute it and/or modify it     #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51  #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

# File last changed $Date: 2006/07/12 17:03:43 $ by $Author: nlohmann $.
# Version $Revision: 1.19 $

SUBDIRS = src doc tests

.PHONY: binarydistreadme


# flags for the compilers
AM_CXXFLAGS= # -g -pedantic 
AM_CFLAGS=   # -g -pedantic


# all files that require the permission to execute
SHELLSCRIPTS = tests/test1 tests/test2 tests/test3

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
	-rm -f doc/mdate-sh
	-rm -f doc/bpel2owfn
	-rm -f doc/stamp-vti
	-rm -f doc/version.texi
	-rm -f doc/Makefile.in
	-rm -f src/Makefile.in
	-rm -f -r src/.deps
	-rm -f src/bpel2owfn.h.in
	-rm -f tests/Makefile.in
	find -type d -exec chmod 755 {} \;
	find -type f -exec chmod 644 {} \;
	chmod 755 $(SHELLSCRIPTS)
	@echo -----------------------------------------------------
	@echo To recreate installation status enter
	@echo   aclocal
	@echo   autoconf
	@echo   autoheader
	@echo   automake -ac
	@echo


# a README file for a binary distribution
binarydistreadme:
	@echo -e Copyright \(C\) 2005, 2006 Niels Lohmann, Christian Gierds and Dennis > BINARYREADME
	@echo -e Reinert. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e This is a binary distribution of BPEL2oWFN, a tool translating business >> BINARYREADME
	@echo -e processes described in BPEL to open workflow nets \(oWFN\). The sources >> BINARYREADME
	@echo -e of BPEL2oWFN can be downloaded from the website of BPEL2oWFN >> BINARYREADME
	@echo -e \(http://www.informatik.hu-berlin.de/top/tools4bpel/bpel2owfn\). >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e BPEL2oWFN is free software\; you can redistribute it and/or modify it >> BINARYREADME
	@echo -e under the terms of the GNU General Public License as published by the >> BINARYREADME
	@echo -e Free Software Foundation\; either version 2 of the License, or \(at your >> BINARYREADME
	@echo -e option\) any later version. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e BPEL2oWFN is distributed in the hope that it will be useful, but >> BINARYREADME
	@echo -e WITHOUT ANY WARRANTY\; without even the implied warranty of >> BINARYREADME
	@echo -e MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU >> BINARYREADME
	@echo -e General Public License for more details. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e You should have received a copy of the GNU General Public License along >> BINARYREADME
	@echo -e with BPEL2oWFN\; if not, write to the Free Software Foundation, Inc., 51 >> BINARYREADME
	@echo -e Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. >> BINARYREADME


# target to create an executable that is independet of Cygwin
pure-windows-executable:
	make hu-binarydist OS=win32 CXXFLAGS="-mno-cygwin -U _WIN32" CFLAGS="-mno-cygwin -U _WIN32" 


# name of the directory/archive for a binary distribution
BINARYDISTDIR = $(distdir)-$(OS)


# files that are distributed in a binary distribution
BINARYDISTFILES = src/$(PACKAGE)$(EXEEXT) doc/$(PACKAGE).pdf COPYING

DOCFILES = doc/$(PACKAGE).pdf
BINARYFILES = src/$(PACKAGE)$(EXEEXT) COPYING

hu-docdist: dvi pdf
	$(mkdir_p) $(BINARYDISTDIR)
	cp $(DOCFILES) $(BINARYDISTDIR)
	zip -9 -m -r $(BINARYDISTDIR).zip $(BINARYDISTDIR)
	
hu-bindist: binarydistreadme
	make --directory=src clean
	make --directory=src
	$(mkdir_p) $(BINARYDISTDIR)
	cp $(BINARYFILES) $(BINARYDISTDIR)
	mv BINARYREADME $(BINARYDISTDIR)/README
	zip -9 -m -r $(BINARYDISTDIR).zip $(BINARYDISTDIR)


# target to create a distribution for an operation system named OS
# (set the variable OS by calling "make hu-binarydist OS=linux")
hu-binarydist: hu-docdist hu-bindist


# target to create a pure windows binary distribution
hu-win32dist: pure-windows-executable
