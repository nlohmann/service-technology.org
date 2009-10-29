bin_PROGRAMS = fiona

fiona_SOURCES = \
	tool_syntax.yy tool_lexic.ll \
	main.cc \
	mainwindow.cc mainwindow.h \
	monitorwidget.cc monitorwidget.h \
	projectswidget.cc projectswidget.h \
	projectview.cc projectview.h \
	node.cc node.h \
	link.cc link.h \
	tool.cc tool.h \
	option.cc option.h \
	importwizard.cc importwizard.h
	

# This section names all files that have to be generated (ui_*.h, moc_*.cc)
nodist_fiona_SOURCES = \
	moc_mainwindow.cpp \
	moc_monitorwidget.cpp \
	moc_projectswidget.cpp \
	moc_projectview.cpp \
	moc_tool.cpp \
	moc_option.cpp \
	moc_importwizard.cpp

# Ensure that those files are in the dist tarball (including generated 
# moc- and ui-files. Resource files must be named, too.
EXTRA_DIST = 

# This rule lets GNU make create any moc_*.cc from the equivalent *.h
moc_%.cpp: %.h
	$(QMAKE) -project -o fiona.pro
	$(QMAKE) -o Makefile.qt
	$(MAKE) -f Makefile.qt

# This rule lets GNU make create the resources.cc from resources.qrc file
#resources.cc : resources.qrc qrc/*.png
#	rcc resources.qrc -o resources.cc

# This line ensures that generated moc_*.cpp, ui_*.h and resources.cc files are
# deleted when we make clean.
CLEANFILES = moc_*.cpp tool_lexic.cc tool_syntax.cc tool_syntax.h
