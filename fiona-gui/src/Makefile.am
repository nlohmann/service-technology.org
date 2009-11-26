include $(top_srcdir)/build-aux/autotroll.mk

bin_PROGRAMS = fiona

fiona_SOURCES = \
  tool_syntax.yy tool_lexic.ll \
  main.cc \
  mainwindow.cc mainwindow.h mainwindow.moc.cc \
  importwizard.cc importwizard.h importwizard.moc.cc\
  link.cc link.h \
  monitorwidget.cc monitorwidget.h monitorwidget.moc.cc \
  node.cc node.h node.moc.cc \
  option.cc option.h option.moc.cc \
  projectswidget.cc projectswidget.h projectswidget.moc.cc \
  projectview.cc projectview.h projectview.moc.cc \
  tool.cc tool.h tool.moc.cc

fiona_CXXFLAGS = $(QT_CXXFLAGS) #$(AM_CXXFLAGS)
fiona_CPPFLAGS = $(QT_CPPFLAGS) #$(AM_CPPFLAGS)
fiona_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS)
fiona_LDADD    = $(QT_LIBS) $(LDADD)

BUILT_SOURCES = \
  mainwindow.moc.cc \
  importwizard.moc.cc \
  monitorwidget.moc.cc \
  node.moc.cc \
  option.moc.cc \
  projectswidget.moc.cc \
  projectview.moc.cc \
  tool.moc.cc

EXTRA_DIST = tool_lexic.cc tool_syntax.cc tool_syntax.h

CLEANFILES = *.moc.cc
