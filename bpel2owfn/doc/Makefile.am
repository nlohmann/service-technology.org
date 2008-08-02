#############################################################################
# GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models
#
# Copyright (C) 2006, 2007  Niels Lohmann,
#                           Christian Gierds, and
#                           Martin Znamirowski
# Copyright (C) 2005        Niels Lohmann and
#			    Christian Gierds
#
# GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.
#
# GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# GNU BPEL2oWFN; if not, see http://www.gnu.org/licenses or write to the Free
# Software Foundation,Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA.
#############################################################################

# File last changed $Date: 2007/06/28 07:38:13 $ by $Author: nielslohmann $.
# Version $Revision: 1.26 $


info_TEXINFOS = bpel2owfn.texi

bpel2owfn_TEXINFOS = fdl.texi ChangeLog.texi \
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

TEXINFO_TEX = texinfo.tex

EXTRA_DIST = gendocs.sh gendocs_template semantics.pdf


#############################################################################
# create ASCII ChangeLog out of Texinfo
#############################################################################
../ChangeLog: ChangeLog.texi
	$(MAKEINFO) -o ChangeLog.txt --no-split --no-headers --fill-column=78 bpel2owfn.texi
	$(SED) -n '/procrastination/,/The most recent change log/w ChangeLog2.txt' ChangeLog.txt
	$(SED) '1,2d' ChangeLog2.txt | $(SED) -e '$$d' | $(SED) -e '$$d' > ChangeLog.txt
	rm ChangeLog2.txt
	head -n8 ../README > ../ChangeLog
	cat ChangeLog.txt >> ../ChangeLog
	rm ChangeLog.txt


#############################################################################
# run the "gendocs.sh" script to create a manual in several formats
#############################################################################
manual: bpel2owfn.texi
	./gendocs.sh $(PACKAGE) "$(PACKAGE_NAME)"
