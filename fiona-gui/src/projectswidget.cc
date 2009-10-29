#include "option.h"
#include "projectswidget.h"
#include "tool.h"
#include "tool_syntax.h"

#include <fstream>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>


ProjectsWidget::ProjectsWidget(QWidget * parent) :
  QTabWidget(parent)
{
}


ProjectsWidget::~ProjectsWidget()
{
}


void ProjectsWidget::newProject(const QString & title)
{
  ProjectView * view = new ProjectView();
  QGraphicsScene * scene = new QGraphicsScene();
  view->setScene(scene);
  view->repaint();
  view->show();

  connect(view, SIGNAL(showDockWidget(Tool *)), this, SLOT(dockWidget(Tool *)));

  addTab(view, QIcon(), title);
}


void ProjectsWidget::openProject(const QString & path)
{
  if (path == "")
    return;
  QString p = path;
  newProject(p.remove(p.length()-5, 5));
}


bool ProjectsWidget::closeProject()
{
  if (currentIndex() < 0)
    return false;
  ProjectView * cView = (ProjectView *) currentWidget();
  QGraphicsScene *cScene = cView->scene();

  removeTab(currentIndex());

  delete cView;
  delete cScene;

  return true;
}


void ProjectsWidget::closeAllProjects()
{
  while (closeProject());
}


void ProjectsWidget::addNode()
{
  ProjectView * cView = (ProjectView *) currentWidget();
  cView->addNode();
}


void ProjectsWidget::addLink()
{
  ProjectView * cView = (ProjectView *) currentWidget();
  cView->addLink();
}


void ProjectsWidget::revertLink()
{
  ProjectView * cView = (ProjectView *) currentWidget();
  cView->revertLink();
}


void ProjectsWidget::makeMakefile()
{
  QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Makefile"), ".");

  ProjectView * cView = (ProjectView *) currentWidget();
  QString makefile = cView->getMakefile();
  std::ofstream file(saveFile.toStdString().c_str());
  file << makefile.toStdString();
  file.close();
}


void ProjectsWidget::createItem(const QString & name, const QString & path)
{
  ProjectView * cView = (ProjectView *) currentWidget();
  Node * n = cView->addNode();
  n->setText(name);
  n->createTool(path);
}


void ProjectsWidget::dockWidget(Tool * t)
{
  emit showDockWidget(t);
}
