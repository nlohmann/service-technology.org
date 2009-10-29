#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "projectview.h"
#include <QtCore/QString>
#include <QtGui/QTabWidget>


class ProjectsWidget : public QTabWidget
{
  Q_OBJECT

public:
  ProjectsWidget(QWidget * = 0);
  ~ProjectsWidget();

public slots:
  void newProject(const QString & = "untitled project");
  void openProject(const QString & = "");
  bool closeProject();
  void closeAllProjects();
  //void saveProject();
  //void saveAsProject();

  void makeMakefile();

  void addNode();
  void addLink();
  void revertLink();

  void createItem(const QString &, const QString &);

  void dockWidget(Tool *);

signals:
  void showDockWidget(Tool *);

};

#endif /* PROJECTSWIDGET_H */
