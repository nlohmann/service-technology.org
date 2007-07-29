TEMPLATE     = app
TARGET       = BPEL2oWFN 

CONFIG       += i386 #static #ppc

FORMS        += dialog.ui

TRANSLATIONS += translation_de.ts

#RC_FILE      += icons/bpel2owfn.icns
ICON         += icons/bpel2owfn.icns
RESOURCES    += BPEL2oWFN.qrc

HEADERS      += gui_organizer.h ../src/helpers.h
SOURCES      += bpel2owfn-gui.cc gui_organizer.cc ../src/helpers.cc
