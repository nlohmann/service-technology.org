#include <QApplication>

#include "ui_dialog.h"
#include "gui_organizer.h"


int main(int argc, char *argv[])
{
  GUI_Organizer organizer;
  
  // the GUI application
  QApplication app(argc, argv);
  
  // the main window widget
  QMainWindow widget;
  
  // the derived (customized) main window
  Ui_MainWindow test;
  
  // set up the customized main window
  test.setupUi(&widget);
  
  
  // the slider for the structural reduction
  QObject::connect(test.reduction_horizontalSlider, SIGNAL(valueChanged(int)), &organizer, SLOT(set_reductionLevel(int)));
  QObject::connect(&organizer, SIGNAL(reductionLevel_changed(QString)), test.reduction_label, SLOT(setText(QString)));

  QObject::connect(test.commandLine_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_longOptions(bool)));
  QObject::connect(&organizer, SIGNAL(commandLine_changed(QString)), test.commandLine_lineEdit, SLOT(setText(QString)));
  
  // show the main window widget
  widget.show();
  
  return app.exec();
}
