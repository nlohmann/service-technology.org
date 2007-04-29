#############################################################################
# Copyright (C) 2005, 2006, 2007 Niels Lohmann                              #
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

# File last changed $Date: 2007/04/29 12:04:30 $ by $Author: nielslohmann $.
# Version $Revision: 1.25 $


# the manual
info_TEXINFOS = bpel2owfn.texi

bpel2owfn_TEXINFOS = fdl.texi \
		     images/bpel2owfn.eps \
		     images/example.eps \
		     images/toolchain.eps \
		     images/tools4bpel.eps \
		     images/ra1.eps \
		     images/ra2.eps \
		     images/rb1.eps \
		     images/rb2.eps \
		     images/diagram.eps \
		     images/bpel2owfn.pdf \
		     images/example.pdf \
		     images/toolchain.pdf \
		     images/tools4bpel.pdf \
		     images/ra1.pdf \
		     images/ra2.pdf \
		     images/rb1.pdf \
		     images/rb2.pdf \
		     images/diagram.pdf \
		     images/bpel2owfn.png \
		     images/example.png \
		     images/toolchain.png \
		     images/tools4bpel.png \
		     images/ra1.png \
		     images/ra2.png \
		     images/rb1.png \
		     images/rb2.png \
		     images/diagram.png

# the installation manual
install_TEXINFOS = install.texi

CLEANFILES = install.aux \
	     install.dvi \
	     install.ky \
	     install.pdf \
	     install.toc \
	     install.vr \
	     install.cp \
	     install.fn \
	     install.log \
	     install.pg \
	     install.tmp \
	     install.cps \
	     install.tp \
	     *~ \
	     -fr manual

EXTRA_DIST = install.texi gendocs.sh gendocs_template


# rules to create the distribution documentation
hu-createdistdocs: install.texi
	$(MAKEINFO) $< -oinstall.txt --no-split --no-headers --no-number-section
	sed -n '/BEGIN OF README/,/END OF README/w ../README' install.txt
	sed -n '/BEGIN OF ChangeLog/,/END OF ChangeLog/w ../ChangeLog' install.txt
	sed -n '/BEGIN OF AUTHORS/,/END OF AUTHORS/w ../AUTHORS' install.txt
	sed -n '/BEGIN OF INSTALL/,/END OF INSTALL/w ../INSTALL' install.txt
	rm install.txt
	./gendocs.sh bpel2owfn "$(PACKAGE_STRING)"
