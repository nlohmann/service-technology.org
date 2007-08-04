#include <QtPlugin>

#include "eventhandler.h"
#include "eventhandler_widgetplugin.h"


EventHandlerPlugin::EventHandlerPlugin(QObject *parent) : QObject(parent)
{
  initialized = false;
}

void EventHandlerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
    return;
  
  initialized = true;
}

bool EventHandlerPlugin::isInitialized() const
{
  return initialized;
}

QWidget *EventHandlerPlugin::createWidget(QWidget *parent)
{
  return new EventHandler(parent);
}

QString EventHandlerPlugin::name() const
{
  return "EventHandler";
}

QString EventHandlerPlugin::group() const
{
  return "Display Widgets [Examples]";
}

QIcon EventHandlerPlugin::icon() const
{
  return QIcon();
}

QString EventHandlerPlugin::toolTip() const
{
  return "";
}

QString EventHandlerPlugin::whatsThis() const
{
  return "";
}

bool EventHandlerPlugin::isContainer() const
{
  return false;
}

QString EventHandlerPlugin::domXml() const
{
  return "<widget class=\"EventHandler\" name=\"horst\">\n"
  " <property name=\"geometry\">\n"
  "  <rect>\n"
  "   <x>0</x>\n"
  "   <y>0</y>\n"
  "   <width>200</width>\n"
  "   <height>200</height>\n"
  "  </rect>\n"
  " </property>\n"
  " <property name=\"toolTip\" >\n"
  "  <string>The current time</string>\n"
  " </property>\n"
  " <property name=\"whatsThis\" >\n"
  "  <string>The horst widget displays "
  "the current time.</string>\n"
  " </property>\n"
  "</widget>\n";
}

QString EventHandlerPlugin::includeFile() const
{
  return "horst.h";
}


// Finally, we use the Q_EXPORT_PLUGIN2() macro to export the AnalogClockPlugin
// class for use with Qt Designer. This macro ensures that Qt Designer can
// access and construct the custom widget. Without this macro, there is no way
// for Qt Designer to use the widget.
Q_EXPORT_PLUGIN2(customwidgetplugin, EventHandlerPlugin)