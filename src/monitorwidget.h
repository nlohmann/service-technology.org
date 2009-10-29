#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QtGui/QTabWidget>


class MonitorWidget : public QTabWidget
{
  Q_OBJECT

public:
  MonitorWidget(QWidget * = 0);
  ~MonitorWidget();

};

#endif /* MONITORWIDGET_H */
