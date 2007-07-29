# type of the built
TEMPLATE     = app

# name of the application
TARGET       = BPEL2oWFN 

# configuration details
CONFIG       += static release

# the UI from the QT Designer
FORMS        += dialog.ui

# translation files (not used yet)
TRANSLATIONS += translation_de.ts

# the icon used for the application ("ICON" does not work!)
RC_FILE      += icons/bpel2owfn.icns

# a file containing information about used resources (icons, graphics)
RESOURCES    += BPEL2oWFN.qrc

# the C++ source files
HEADERS      += gui_organizer.h ../src/helpers.h
SOURCES      += bpel2owfn-gui.cc gui_organizer.cc ../src/helpers.cc
