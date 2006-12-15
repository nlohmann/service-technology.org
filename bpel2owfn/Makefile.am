#############################################################################
# Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      #
#                                                                           #
# This file is part of GNU BPEL2oWFN.                                       #
#                                                                           #
# GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  #
# under the terms of the GNU General Public License as published by the     #
# Free Software Foundation; either version 2 of the License, or (at your    #
# option) any later version.                                                #
#                                                                           #
# GNU BPEL2oWFN is distributed in the hope that it will be useful, but      #
# WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  #
# Public License for more details.                                          #
#                                                                           #
# You should have received a copy of the GNU General Public License along   #
# with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  #
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. #
#############################################################################

# File last changed $Date: 2006/12/15 19:06:42 $ by $Author: nielslohmann $.
# Version $Revision: 1.38 $

SUBDIRS = src tests doc

.PHONY: binarydistreadme update


dist-hook:
	rm -rf `find $(distdir) -name CVS`


####################################
# Targets to clean generated files #
####################################

CLEANFILES = -r *~

# all files that require the permission to execute
SHELLSCRIPTS = tests/test1 tests/test2 tests/test3 tests/test4 tests/test5

# delete all files that can be created by other files
hu-prepare: maintainer-clean
	-rm -f COPYING
	-rm -f aclocal.m4
	-rm -f configure
	-rm -f configure
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f ylwrap
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


####################################
# Targets for binary distributions #
####################################

OS = $(build_cpu)-$(build_os)

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

# name of the directory/archive for a binary distribution
BINARYDISTDIR = $(distdir)-$(OS)

# set OS for the directory-prefix
binarydist: dvi pdf binarydistreadme
	$(MAKE) --directory=src clean
	$(MAKE) --directory=src
	$(mkdir_p) $(BINARYDISTDIR)
	cp doc/$(PACKAGE).pdf $(BINARYDISTDIR)
	cp src/$(PACKAGE)$(EXEEXT) COPYING $(BINARYDISTDIR)
	mv BINARYREADME $(BINARYDISTDIR)/README
	$(AMTAR) -cf $(BINARYDISTDIR).tar $(BINARYDISTDIR)
	gzip $(BINARYDISTDIR).tar
	rm -fr $(BINARYDISTDIR)

# target to create a Mac Universal binary
bindist_universal:
	echo "you have to configure with:"
	echo "  ./configure --disable-dependency-tracking"
	$(MAKE) binarydist OS=mac-universal CXXFLAGS="-O3 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \
	  CFLAGS="-O3 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \
	  LDFLAGS="-arch i386 -arch ppc"

# target to create an executable that is independet of Cygwin
bindist_win32:
	$(MAKE) binarydist OS=$(build_cpu)-mingw32 CXXFLAGS="-mno-cygwin -U _WIN32 -O3" CFLAGS="-mno-cygwin -U _WIN32 -O3"

# target for any other operating system
bindist:
	$(MAKE) binarydist CXXFLAGS="-O3" CFLAGS="-O3"


###############################################
# Targets for automatic upload to ftp.gnu.org #
###############################################

gnupload_alpha: *.tar.gz
	@echo "====================================="
	@echo " FILE DISTRIBUTION TO GNU FTP SERVER "
	@echo "====================================="
	@ls *.tar.gz -1
	@echo
	@echo Are you sure you want to upload these files to ftp://alpha.gnu.org/bpel2owfn?
	@echo "If not, please press (crtl+c) now."
	@read answer
	gnupload --to alpha.gnu.org:bpel2owfn $^
	rm $(^:.tar.gz=.tar.gz.sig)

gnupload_release: *.tar.gz
	@echo "============================================="
	@echo " RELEASE FILE DISTRIBUTION TO GNU FTP SERVER "
	@echo "============================================="
	@ls *.tar.gz -1
	@echo
	@echo Are you sure you want to upload these files to ftp://ftp.gnu.org/bpel2owfn?
	@echo "If not, please press (crtl+c) now."
	@read answer
	gnupload --to ftp.gnu.org:bpel2owfn $^
	rm $(^:.tar.gz=.tar.gz.sig)


update:
	@echo "====================================="
	@echo " UPDATE VIA ANONYMOUS CVS REPOSITORY "
	@echo "====================================="
	@echo
	@echo Are you sure you want to check out the latest source code from cvs.savannah.gnu.org?
	@echo "If not, please press (crtl+c) now."
	@read answer
	$(MAKE) clean
	cvs -z3 -d:pserver:anonymous@cvs.savannah.gnu.org:/sources/bpel2owfn co -d newsources bpel2owfn
	rm -rf `find $(abs_top_srcdir)/newsources -name CVS`
	cp $(abs_top_srcdir)/newsources/* $(abs_top_srcdir) -R -u -v
	rm -fr $(abs_top_srcdir)/newsources
