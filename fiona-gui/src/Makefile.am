bin_PROGRAMS = fiona

# Some trick to let qmake do the job
all:
  $(QMAKE) -project -o fiona.pro
  $(QMAKE) -o Makefile.qt
  $(MAKE) -f Makefile.qt

# Ensure that those files are in the dist tarball (including generated 
# moc- and ui-files. Resource files must be named, too.
EXTRA_DIST = \
  main.cc \
  mainwindow.cc mainwindow.h \
  monitorwidget.cc monitorwidget.h \
  projectswidget.cc projectswidget.h \
  projectview.cc projectview.h \
  node.cc node.h \
  link.cc link.h \
  tool.cc tool.h \
  option.cc option.h \
  importwizard.cc importwizard.h \
  tool_syntax.yy tool_syntax.cc tool_syntax.h \
  tool_lexic.ll tool_syntax.cc tool_syntax.h
 

# This rule lets GNU make create any moc_*.cc from the equivalent *.h


# This rule lets GNU make create the resources.cc from resources.qrc file
#resources.cc : resources.qrc qrc/*.png
#	rcc resources.qrc -o resources.cc

# This line ensures that generated moc_*.cpp, ui_*.h and resources.cc files are
# deleted when we make clean.
CLEANFILES = moc_*.cpp Makefile.qt fiona.pro
