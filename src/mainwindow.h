#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "importwizard.h"
#include "monitorwidget.h"
#include "projectswidget.h"
#include "tool.h"
#include <QtCore/QVector>
#include <QtGui/QtGui>


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void newProject();
  void openProject();
  void closeProject();
  void closeAllProjects();
  //void saveProject();
  //void saveAsProject();
  void importTool();
  void browseFiles();
  void createItem();
  void showDockWidget(Tool *);
  void createToolButton(const QString &, const QString &, const QString &);

signals:
  void browsedFiles(const QString &);

private:
  void setupMenu();
  void setupToolBox();

private:
  /// main items
  QToolBox * tools_;
  ProjectsWidget * projects_;
  MonitorWidget * monitor_;
  QWidget * projectDetails_;

  /// link button
  QToolButton * linkButton_;
  QToolButton * revertButton_;

  QDockWidget * toolSetup_;
  QVector<QScrollArea *> toolScroll_;

  /// menu items
  QMenu * fileMenu_;
  QAction * actionNew_;
  QAction * actionOpen_;
  QAction * actionClose_;
  QAction * actionCloseAll_;
  QAction * actionSave_;
  QAction * actionSaveAs_;
  QAction * actionImport_;
  QAction * actionQuit_;
  QMenu * editMenu_;
  QMenu * viewsMenu_;
  QMenu * projectMenu_;
  QAction * actionMakeMakefile_;
  QMenu * helpMenu_;

  QVector<QToolButton *> buttons_;

  /// layouts
  QGridLayout * centerLayout_;

  ImportWizard * import_;

};

#endif // MAINWINDOW_H
