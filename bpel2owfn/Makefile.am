# tell aclocal about the macros in the "m4" folder
ACLOCAL_AMFLAGS = -I m4 --install

# Where else are Makefiles?
SUBDIRS = src tests doc man


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


svn-clean: maintainer-clean
	rm -fr configure INSTALL aclocal.m4 src/config.h.in doc/mdate-sh src/config-log.h build-aux
	for DIR in $(DIST_SUBDIRS) .; do rm -f $$DIR/Makefile.in; done
