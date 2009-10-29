#include "mainwindow.h"

#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MainWindow w;
  w.show();

  return app.exec();
}
