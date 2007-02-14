############################################################################
# Copyright 2005, 2006 Daniela Weinberg, Peter Massuthe                    #
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

# File last changed $Date: 2007-02-14 13:20:01 $ by $Author: weinberg $.
# Version $Revision: 1.1 $

info_TEXINFOS = fiona.texi

fiona_TEXINFOS = gpl.texi \
		     images/fiona.eps \
		     images/fiona.pdf \
		     images/fiona.png \
		     images/tools4bpel.eps \
		     images/tools4bpel.pdf \
		     images/tools4bpel.png


# rules for the installation manual
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
	     install.tp

EXTRA_DIST = install.texi

# rules to create the distribution documentation
hu-createdistdocs: install.texi
	$(MAKEINFO) $< -oinstall.txt --no-split --no-headers --no-number-section
	sed -n '/BEGIN OF README/,/END OF README/w ../README' install.txt
	sed -n '/BEGIN OF ChangeLog/,/END OF ChangeLog/w ../ChangeLog' install.txt
	sed -n '/BEGIN OF AUTHORS/,/END OF AUTHORS/w ../AUTHORS' install.txt
	sed -n '/BEGIN OF INSTALL/,/END OF INSTALL/w ../INSTALL' install.txt
	rm install.txt

