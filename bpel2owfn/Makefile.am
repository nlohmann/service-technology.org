#############################################################################
# Copyright (C) 2007       Niels Lohmann                                    #
# Copyright (C) 2005, 2006 Niels Lohmann, Christian Gierds                  #
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

# File last changed $Date: 2007/04/20 12:25:24 $ by $Author: nielslohmann $
# Version $Revision: 1.55 $

# Where else are Makefiles?
SUBDIRS = src tests doc rpm

# Which targets have no dependencies (i.e. always succeed)?
.PHONY: binarydistreadme update

# do not publish CVS files
dist-hook:
	rm -rf `find $(distdir) -name CVS`


####################################
# Targets to clean generated files #
####################################

# always remove vi's temporary files
CLEANFILES = -r *~

# all files that require the permission to execute
SHELLSCRIPTS = tests/test1 \
	       tests/test2 \
	       tests/test3 \
	       tests/test4 \
	       tests/test5

# Delete all files that can be created by other files, repair file
# permissions and set filetype to UNIX.
hu-prepare: maintainer-clean
	-rm -f COPYING
	-rm -f aclocal.m4
	-rm -f configure
	-rm -f configure
	-rm -f depcomp
	-rm -f compile
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
	-rm -f src/bpel2owfn.h.in
	-rm -f -r src/.deps
	-rm -f tests/Makefile.in
	-rm -f rpm/Makefile.in	
	find -type d -exec chmod 755 {} \;
	find -type f -exec chmod 644 {} \;
	-find -type f -exec dos2unix {} \;
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

# an operating system string (composed of information from configure)
OS = $(build_cpu)-$(build_os)

# a README file for a binary distribution
binarydistreadme:
	@echo -e Copyright \(C\) 2007 Niels Lohmann, Christian Gierds, and Martin Znamirowski > BINARYREADME
	@echo -e Copyright \(C\) 2005, 2006 Niels Lohmann and Christian Gierds >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e This is a binary distribution of @PACKAGE_NAME@ @PACKAGE_VERSION@, a tool translating >> BINARYREADME
	@echo -e business processes described in BPEL to open workflow nets \(oWFN\). >> BINARYREADME
	@echo -e The sources of GNU BPEL2oWFN can be downloaded from the website of >> BINARYREADME
	@echo -e GNU BPEL2oWFN \(http://www.gnu.org/software/bpel2owfn\). >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e GNU BPEL2oWFN is free software\; you can redistribute it and/or modify it >> BINARYREADME
	@echo -e under the terms of the GNU General Public License as published by the >> BINARYREADME
	@echo -e Free Software Foundation\; either version 2 of the License, or \(at your >> BINARYREADME
	@echo -e option\) any later version. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e GNU BPEL2oWFN is distributed in the hope that it will be useful, but >> BINARYREADME
	@echo -e WITHOUT ANY WARRANTY\; without even the implied warranty of >> BINARYREADME
	@echo -e MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU >> BINARYREADME
	@echo -e General Public License for more details. >> BINARYREADME
	@echo >> BINARYREADME
	@echo -e You should have received a copy of the GNU General Public License along >> BINARYREADME
	@echo -e with GNU BPEL2oWFN\; if not, write to the Free Software Foundation, Inc., 51 >> BINARYREADME
	@echo -e Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. >> BINARYREADME

# name of the directory/archive for a binary distribution
BINARYDISTDIR = $(distdir)-$(OS)

# create a binary distribution tarball consisting of a binary executable of
# the respective system, a PDF documentation and the README file for binary
# distributions (see target "binarydistreadme").
binarydist: pdf binarydistreadme
	$(MAKE) --directory=src clean
	$(MAKE) --directory=src CXXFLAGS+="-O3 -D NDEBUG" CFLAGS+="-O3 -D NDEBUG" # use optimization
	$(mkdir_p) $(BINARYDISTDIR)
	cp doc/$(PACKAGE).pdf $(BINARYDISTDIR)
	cp src/$(PACKAGE)$(EXEEXT) COPYING $(BINARYDISTDIR)
	mv BINARYREADME $(BINARYDISTDIR)/README
	$(AMTAR) -cf $(BINARYDISTDIR).tar $(BINARYDISTDIR)
	gzip $(BINARYDISTDIR).tar
	rm -fr $(BINARYDISTDIR)

# target to create a Mac Universal binary
bindist_universal:
	@echo "you have to configure with:"
	@echo "  ./configure --disable-dependency-tracking"
	$(MAKE) binarydist OS=mac-universal CXXFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \
	  CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \
	  LDFLAGS="-arch i386 -arch ppc"

# Target to create an executable that is independet of Cygwin (i.e. uses the
# MinGW libraries insread of Cygwin's own libraries. Whether the compilation
# succeeded can be checked with "cygcheck ./bpel2owfn.exe", which displays all
# library dependencies.
bindist_win32:
	$(MAKE) binarydist OS=$(build_cpu)-win32 CXXFLAGS="-mno-cygwin -U _WIN32" CFLAGS="-mno-cygwin -U _WIN32"

# target for any other operating system
bindist:
	$(MAKE) binarydist CXXFLAGS="-O3" CFLAGS="-O3"


#################################################################
# Targets for automatic upload to ftp.gnu.org and alpha.gnu.org #
#################################################################

gnupload_alpha: *.tar.gz
	@echo "====================================="
	@echo " FILE DISTRIBUTION TO GNU FTP SERVER "
	@echo "====================================="
	@ls *.tar.gz -1
	@echo
	@echo "Are you sure you want to upload these files to ftp://alpha.gnu.org/gnu/bpel2owfn?"
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
	@echo "Are you sure you want to upload these files to ftp://ftp.gnu.org/gnu/bpel2owfn?"
	@echo "If not, please press (crtl+c) now."
	@read answer
	gnupload --to ftp.gnu.org:bpel2owfn $^
	rm $(^:.tar.gz=.tar.gz.sig)


###################################################
# Targets for automatic update from alpha.gnu.org #
###################################################

update:
	@echo "=========================="
	@echo " UPDATE VIA ANONYMOUS FTP "
	@echo "=========================="
	@echo
	@echo "Are you sure you want to check out the latest source code"
	@echo "from ftp://alpha.gnu.org/gnu/bpel2owfn?"
	@echo "If not, please press (crtl+c) now."
	@read answer
	$(MAKE) clean
	-rm bpel2owfn-*.tar.gz
	@echo "open alpha.gnu.org" > ftpcommands
	@echo "anonymous" >> ftpcommands
	@echo "cd gnu/bpel2owfn" >> ftpcommands
	@echo "binary" >> ftpcommands
	@echo "mget *.tar.gz" >> ftpcommands
	@echo "y" >> ftpcommands
	@echo "disconnect" >> ftpcommands
	@echo "bye" >> ftpcommands
	@echo "" >> ftpcommands
	ftp < ftpcommands
	mv *.tar.gz bpel2owfn-experimental.tar.gz
	gunzip bpel2owfn-experimental.tar.gz
	tar xf bpel2owfn-experimental.tar
	rm bpel2owfn-experimental.tar
	$(abs_top_srcdir)/bpel2owfn-*/configure
	$(MAKE)


#####################################
# Targets to create and RPM package #
#####################################

#  Produce an rpm package using dist or from the given tarball
#  Usage:
#    make rpm-dist
#    make release=1 rpm-dist
#    make rpm-this
#    make version=2.3.22 release=2 rpm-this
#    make rpm-dist cparams='--enable-kanji --quiet' mparams='CFLAGS="-O2 -g"'

rpm-dist: dist rpm-this

rpm-this:
	(cd rpm && $(MAKE) $(AM_MAKEFLAGS) rpm) || exit 1
