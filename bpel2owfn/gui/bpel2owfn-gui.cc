#include <QApplication>

#include "ui_dialog.h"
#include "gui_organizer.h"


int main(int argc, char *argv[])
{  
  // the GUI application
  QApplication app(argc, argv);
  
  // the main window widget
  QMainWindow widget;
  
  // the derived (customized) main window
  Ui_MainWindow test;
  
  // set up the customized main window
  test.setupUi(&widget);
  
  // the organizer object
  GUI_Organizer organizer;
  organizer.fileDialog = test.StandardFileDialog;
  
  
  // the slider for the structural reduction
  QObject::connect(test.reduction_horizontalSlider, SIGNAL(valueChanged(int)), &organizer, SLOT(set_reductionLevel(int)));
  QObject::connect(&organizer, SIGNAL(reductionLevel_changed(QString)), test.reduction_label, SLOT(setText(QString)));

  // the command line
  QObject::connect(test.commandLine_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_longOptions(bool)));
  QObject::connect(&organizer, SIGNAL(commandLine_changed(QString)), test.commandLine_lineEdit, SLOT(setText(QString)));
  
  // the output file
  QObject::connect(test.fileFormatFiona_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_fileFormatFiona(bool)));  
  QObject::connect(test.fileFormatLola_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_fileFormatLola(bool)));  
  QObject::connect(test.fileFormatPnml_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_fileFormatPnml(bool)));  
  QObject::connect(test.fileFormatDot_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_fileFormatDot(bool)));  
  QObject::connect(test.fileFormat_lineEdit, SIGNAL(textChanged(QString)), &organizer, SLOT(set_outputFileName(QString)));

  // the input file
  QObject::connect(test.inputFile_lineEdit, SIGNAL(textChanged(QString)), &organizer, SLOT(set_inputFileName(QString)));
  
  // the patterns
  QObject::connect(test.patternSmall_radioButton, SIGNAL(toggled(bool)), &organizer, SLOT(set_patternSmall(bool)));
  QObject::connect(test.patternNofhfaults_radioButton, SIGNAL(toggled(bool)), &organizer, SLOT(set_patternNofhfaults(bool)));
  QObject::connect(test.patternStandardfaults_radioButton, SIGNAL(toggled(bool)), &organizer, SLOT(set_patternStandardfaults(bool)));  
  QObject::connect(test.patternVariables_checkBox, SIGNAL(toggled(bool)), &organizer, SLOT(set_patternVariables(bool)));  
  QObject::connect(&organizer, SIGNAL(no_variables_possible(bool)), test.patternVariables_checkBox, SLOT(setEnabled(bool)));
  
  // the translate button
  QObject::connect(test.translate_pushButton, SIGNAL(pressed()), &organizer, SLOT(translateButton_pressed()));

  // the browse button
  QObject::connect(test.inputFile_pushButton, SIGNAL(pressed()), &organizer, SLOT(browseButton_pressed()));
  QObject::connect(&organizer, SIGNAL(inputFileName_changed(QString)), test.inputFile_lineEdit, SLOT(setText(QString)));
  
  // error messages
  QObject::connect(&organizer, SIGNAL(show_errormessage(QString)), test.StandardErrorMessage, SLOT(showMessage(QString)));
  
  // initialize the command line
  organizer.set_commandLine();
    
  // show the main window widget
  widget.show();
  

  
  return app.exec();
}
