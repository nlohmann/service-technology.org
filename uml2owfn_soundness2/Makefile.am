###############################################################################
# UML2oWFN -- Translating IBM Business Modeler Processes into Petri Net Models
###############################################################################

# File last changed $Date: 2007/06/28 07:38:13 $ by $Author: nielslohmann $
# Version $Revision: 1.64 $

# Where else are Makefiles?
SUBDIRS = src doc tests maintainer

####################################
# Targets to clean generated files #
####################################

# always remove vi's temporary files
CLEANFILES = -r *~

#############################################################################
# remove everything that can be re-built
#############################################################################
svn-clean: maintainer-clean
	rm -fr .DS_Store INSTALL Makefile.in aclocal.m4 compile configure depcomp \
	  install-sh missing ylwrap $(DIST_ARCHIVES).sig config.sub config.guess \
	  config.h.in~
	rm -fr doc/.DS_Store doc/ChangeLog.txt doc/Makefile.in doc/mdate-sh doc/manual
	rm -fr src/.DS_Store src/Makefile.in src/config.h.in config.h.in~
	rm -fr tests/Makefile.in tests/.DS_Store
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
	
#	rm -fr man/Makefile.in
#	rm -fr exp/Makefile.in exp/bpm2008
#	rm -fr maintainer/Makefile.in
	


####################################
# Targets for binary distributions #
####################################

# an operating system string (composed of information from configure)
OS = $(build_cpu)-$(build_os)

# a README file for a binary distribution
binarydistreadme:
	@echo -e Copyright \(C\) 2007, 2008 Niels Lohmann, Christian Gierds, Martin Znamirowski, and > BINARYREADME
	@echo -e                            Dirk Fahland  >> BINARYREADME
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
	@read answer
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
