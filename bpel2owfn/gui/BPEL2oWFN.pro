TEMPLATE = app
TARGET = BPEL2oWFN 

# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
CONFIG += i386 #ppc

ICON = icons/bpel2owfn.icns

HEADERS += gui_organizer.h ../src/helpers.h
FORMS += dialog.ui
SOURCES += bpel2owfn-gui.cc gui_organizer.cc ../src/helpers.cc
