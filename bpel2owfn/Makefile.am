# Where else are Makefiles?
SUBDIRS = src tests doc maintainer maintainer/gnu man


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
svn-clean: maintainer-clean
	-rm -f aclocal.m4
	-rm -f compile INSTALL
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
	-rm -f -r src/.deps doc/doxygen
	-rm -f tests/Makefile.in
	-rm -f maintainer/Makefile.in maintainer/gnu/Makefile.in
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
