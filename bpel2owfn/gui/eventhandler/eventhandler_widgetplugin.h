#ifndef EVENTHANDLER_WIDGETPLUGIN_H
#define EVENTHANDLER_WIDGETPLUGIN_H

#include <QDesignerCustomWidgetInterface>

class EventHandlerPlugin : public QObject, public QDesignerCustomWidgetInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  
  public:
    /// The class constructor simply calls the QObject base class constructor and sets the initialized variable to false.
    EventHandlerPlugin(QObject *parent = 0);
    
    /// return false as this widget is no container
    bool isContainer() const;
    
    /// The isInitialized() function lets Qt Designer know whether the plugin is ready for use.
    bool isInitialized() const;
    
    /// The icon used to represent the widget in the widget box is returned by the icon() function.
    QIcon icon() const;

    /// The domXml() function provides a way to include default settings for the widget in the standard XML format used by Qt Designer.
    QString domXml() const;
    
    /// The group() function is used to describe the type of widget that the custom widget belongs to.
    QString group() const;

    /// To make the widget usable by applications, we implement the includeFile() function to return the name of the header file containing the custom widget class definition.
    QString includeFile() const;
    
    /// The name() function returns the name of class that provides the custom widget.
    QString name() const;
    
    /// The toolTip() function should return a short message describing the widget.
    QString toolTip() const;
    
    /// The whatsThis() function can return a longer description.
    QString whatsThis() const;
    
    /// Instances of the custom widget are supplied by the createWidget() function.
    QWidget *createWidget(QWidget *parent);
    
    /// Qt Designer will initialize the plugin when it is required by calling the initialize() function.
    void initialize(QDesignerFormEditorInterface *core);
    
  private:
    /// true iff initialize() was called
    bool initialized;
};

#endif