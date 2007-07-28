#ifndef GUI_ORGANIZER_H
#define GUI_ORGANIZER_H

#include <QObject>


class GUI_Organizer : public QObject
{
  Q_OBJECT
  
  private:
    int reduction_level;
    bool use_long_options;
  
  public:
    void set_commandLine();
    GUI_Organizer();
  
  public slots:
    void set_reductionLevel(int value);
    void set_longOptions(bool value);
  
  signals:
    void reductionLevel_changed(QString text);
    void commandLine_changed(QString text);
};

#endif