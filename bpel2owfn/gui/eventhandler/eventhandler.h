#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

// #include <QObject>
#include <QFileDialog>
#include <QWidget>
#include <QtDesigner/QDesignerExportWidget>



class QDESIGNER_WIDGET_EXPORT EventHandler : public QWidget
{
  Q_OBJECT
  
  private:
    int reduction_level;
  
    bool use_long_options;
    
    bool fileFormat_fiona;
    bool fileFormat_lola;
    bool fileFormat_pnml;
    bool fileFormat_dot;
    
    QString output_filename;
    QString input_filename;
    QString command_line;
    
    int patterns;
    bool patterns_variables;
  
  public:
    EventHandler(QWidget *parent = NULL);
    
    QFileDialog *fileDialog;
  
  public slots:
    void set_commandLine();
    void set_reductionLevel(int value);
    void set_longOptions(bool value);
    void set_fileFormatFiona(bool value);
    void set_fileFormatLola(bool value);
    void set_fileFormatPnml(bool value);
    void set_fileFormatDot(bool value);
    void set_outputFileName(QString value);
    void set_patternSmall(bool value);
    void set_patternNofhfaults(bool value);
    void set_patternStandardfaults(bool value);
    void set_patternVariables(bool value);
    void set_inputFileName(QString value);
    void translateButton_pressed();
    void browseButton_pressed();
  
  signals:
    void reductionLevel_changed(QString);
    void commandLine_changed(QString);
    void no_variables_possible(bool);
    void show_errormessage(QString);
    void inputFileName_changed(QString);
};

#endif
