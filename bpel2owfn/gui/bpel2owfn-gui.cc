#include "ui_dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QMainWindow widget;
  Ui_MainWindow test;
  test.setupUi(&widget);
  widget.show();
  return app.exec();
}
