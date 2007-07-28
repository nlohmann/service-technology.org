#ifndef GUI_ORGANIZER_H
#define GUI_ORGANIZER_H

#include <QObject>


class GUI_Organizer : public QObject
{
  Q_OBJECT
  
  public:
    int reduction_level;
    void set_commandLine();
  
  public slots:
    void set_reductionLevel(int value);
  
  signals:
    void reductionLevel_changed(QString text);
    void commandLine_changed(QString text);
};

#endif