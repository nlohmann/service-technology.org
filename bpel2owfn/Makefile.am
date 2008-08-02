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

# Where else are Makefiles?
SUBDIRS = src tests doc maintainer/gnu maintainer/rpm man


##################################
# Targets to manage distribution #
##################################

EXTRA_DIST = gui/bpel2owfn-gui.cc \
             gui/BPEL2oWFN.pmproj \
             gui/BPEL2oWFN.pro \
             gui/BPEL2oWFN.qrc \
             gui/dialog.ui \
             gui/eventhandler/eventhandler.cc \
             gui/eventhandler/eventhandler.h \
             gui/eventhandler/eventhandler_plugin.pro \
             gui/eventhandler/eventhandler_widgetplugin.cc \
             gui/eventhandler/eventhandler_widgetplugin.h \
             gui/gui_organizer.cc \
             gui/gui_organizer.h \
             gui/icons/about.rtf \
             gui/icons/about.rtfd/TXT.rtf \
             gui/icons/background.jpg \
             gui/icons/bpel.png \
             gui/icons/bpel2owfn.icns \
             gui/icons/bpel2owfn.ico \
             gui/icons/bpel2owfn.rc \
             gui/icons/bpel4chor.png \
             gui/icons/gpl.rtf \
             gui/README \
             gui/resource/about.rtf \
             gui/resource/background.jpg \
             gui/resource/bpel.png \
             gui/resource/bpel2owfn.icns \
             gui/resource/bpel2owfn.ico \
             gui/resource/bpel2owfn.rc \
             gui/resource/bpel4chor.png \
             gui/resource/gpl.rtf \
             gui/static_release.sh \
             gui/translation-de.ts


####################################
# Targets to clean generated files #
####################################

# remove everything that can be rebuilt
cvs-clean: maintainer-clean
	-rm -f aclocal.m4
	-rm -f compile COPYING INSTALL
	-rm -f configure config.guess config.sub
	-rm -f depcomp
	-rm -f install-sh
	-rm -f Makefile.in
	-rm -f missing
	-rm -f ylwrap
	-rm -f -r autom4te.cache
	-rm -f doc/mdate-sh
	-rm -f doc/bpel2owfn
	-rm -f doc/stamp-vti
	-rm -f doc/version.texi doc/texinfo.tex
	-rm -f doc/Makefile.in
	-rm -f man/Makefile.in
	-rm -f src/Makefile.in
	-rm -f src/config.h.in src/config.h.in~
	-rm -f -r src/.deps src/doc
	-rm -f tests/Makefile.in
	-rm -f maintainer/rpm/Makefile.in
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
