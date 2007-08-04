 TEMPLATE    = lib
 CONFIG      += designer plugin

 HEADERS     = eventhandler.h \
               eventhandler_widgetplugin.h
 SOURCES     = eventhandler.cc \
               eventhandler_widgetplugin.cc

 target.path = $$[QT_INSTALL_PLUGINS]/designer
 INSTALLS += target
