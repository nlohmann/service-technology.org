#include "mainwindow.h"
#include "tool.h"

#include <QtGui/QtGui>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  setMinimumSize(600, 400);
  /// creating all needed elements
  projects_ = new ProjectsWidget(this);
  monitor_ = new MonitorWidget(this);
  projectDetails_ = new QWidget(this);

  linkButton_ = new QToolButton();
  linkButton_->setText(tr("Link"));
  revertButton_ = new QToolButton();
  revertButton_->setText(tr("Reverse"));
  QFormLayout * linkLayout = new QFormLayout();
  linkLayout->addWidget(linkButton_);
  linkLayout->addWidget(revertButton_);
  projectDetails_->setLayout(linkLayout);

  connect(linkButton_, SIGNAL(clicked()), projects_, SLOT(addLink()));
  connect(revertButton_, SIGNAL(clicked()), projects_, SLOT(revertLink()));
  connect(projects_, SIGNAL(showDockWidget(Tool *)), this, SLOT(showDockWidget(Tool *)));

  setupToolBox();
  setupMenu();

  projectDetails_->setMinimumSize(100, 100);
  projectDetails_->setMaximumSize(200, 150);
  monitor_->setMinimumSize(400, 100);
  monitor_->setMaximumHeight(150);

  toolSetup_ = new QDockWidget(tr("Tool Setup"), this);
  toolSetup_->setAllowedAreas(Qt::RightDockWidgetArea);
  addDockWidget(Qt::RightDockWidgetArea, toolSetup_);
  toolSetup_->setVisible(false);

  setCentralWidget(new QWidget());

  /// setup layouts
  centerLayout_ = new QGridLayout();
  centerLayout_->addWidget(tools_, 0, 0);
  centerLayout_->addWidget(projects_, 0, 1);
  centerLayout_->addWidget(projectDetails_, 1, 0);
  centerLayout_->addWidget(monitor_, 1, 1);
  centralWidget()->setLayout(centerLayout_);

  /// dummy
  projects_->newProject("untitled project");

  import_ = new ImportWizard(this);
  connect(import_, SIGNAL(createToolButton(const QString &, const QString &, const QString &)), this,
      SLOT(createToolButton(const QString &, const QString &, const QString &)));
}

MainWindow::~MainWindow()
{
}


void MainWindow::newProject()
{
  projects_->newProject();
}


void MainWindow::openProject()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open Project"), ".", tr("Projects (*.proj)"));
  projects_->openProject(path);
}


void MainWindow::closeProject()
{
  projects_->closeProject();
}


void MainWindow::closeAllProjects()
{
  projects_->closeAllProjects();
}


void MainWindow::importTool()
{
  import_->restart();
  import_->show();
}


void MainWindow::browseFiles()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Open Gengetopt file"),
      ".", tr("Gengetopt input file (*.ggo)"));

  emit browsedFiles(file);
}


void MainWindow::createItem()
{
  QString path;
  QString name;
  for (int i = 0; i < buttons_.size(); i++)
    if (buttons_[i]->isChecked())
    {
      buttons_[i]->setChecked(false);
      path = buttons_[i]->toolTip();
      name = buttons_[i]->text();
      break;
    }

  projects_->createItem(name, path);
}


void MainWindow::setupMenu()
{
  fileMenu_ = menuBar()->addMenu(tr("&File"));
  actionNew_ = fileMenu_->addAction(QIcon(), tr("&New Project"), this, SLOT(newProject()), tr("Ctrl+N"));
  actionOpen_ = fileMenu_->addAction(QIcon(), tr("&Open"), this, SLOT(openProject()), tr("Ctrl+O"));
  fileMenu_->addSeparator();
  actionClose_ = fileMenu_->addAction(QIcon(), tr("&Close"), this, SLOT(closeProject()));
  actionCloseAll_ = fileMenu_->addAction(QIcon(), tr("Close a&ll"), this, SLOT(closeAllProjects()));
  fileMenu_->addSeparator();
  actionSave_ = fileMenu_->addAction(QIcon(), tr("&Save"));
  actionSaveAs_ = fileMenu_->addAction(QIcon(), tr("Save &as"));
  fileMenu_->addSeparator();
  actionImport_ = fileMenu_->addAction(QIcon(), tr("&Import Tool"), this, SLOT(importTool()));
  fileMenu_->addSeparator();
  actionQuit_ = fileMenu_->addAction(QIcon(), tr("&Quit"), this, SLOT(close()), tr("Ctrl+Q"));

  editMenu_ = menuBar()->addMenu(tr("&Edit"));

  viewsMenu_ = menuBar()->addMenu(tr("&Views"));

  projectMenu_ = menuBar()->addMenu(tr("&Project"));
  actionMakeMakefile_ = projectMenu_->addAction(QIcon(), tr("&Make Makefile"), projects_, SLOT(makeMakefile()));

  helpMenu_ = menuBar()->addMenu(tr("&Help"));
}


void MainWindow::setupToolBox()
{
  QWidget * analysisWidget = new QWidget();

  QWidget * compilersWidget = new QWidget();

  QWidget * correctionWidget = new QWidget();

  QWidget * utilitiesWidget = new QWidget();

  QWidget * editorsSimulatorsWidget = new QWidget();

  QWidget * toolChainsWidget = new QWidget();

  QWidget * miscWidget = new QWidget();

  tools_ = new QToolBox();
  tools_->setMinimumWidth(150);
  tools_->setMaximumWidth(250);
  tools_->addItem(analysisWidget, tr("Analysis"));
  tools_->addItem(compilersWidget, tr("Compilers"));
  tools_->addItem(correctionWidget, tr("Correction"));
  tools_->addItem(utilitiesWidget, tr("Utilities"));
  tools_->addItem(editorsSimulatorsWidget, tr("Editors and Simulators"));
  tools_->addItem(toolChainsWidget, tr("Tool Chains"));
  tools_->addItem(miscWidget, tr("Misc."));
}


void MainWindow::showDockWidget(Tool * t)
{
  int index = toolScroll_.size();
  QWidget * w = t->dockWidgetContents();
  for (int i = 0; i < toolScroll_.size(); i++)
    if (toolScroll_[i]->widget() == w)
      index = i;

  if (index == toolScroll_.size())
  {
    QScrollArea * a = new QScrollArea();
    a->setWidget(w);
    toolScroll_.push_back(a);
  }

  toolScroll_[index]->setWindowTitle(t->package());
  toolScroll_[index]->show();
}


void MainWindow::createToolButton(const QString & name, const QString & category, const QString & path)
{
  int index = tools_->count()-1;
  for (int i = 0; i < tools_->count(); i++)
    if (tools_->itemText(i) == category)
    {
      index = i;
      break;
    }

  QVBoxLayout * layout = (QVBoxLayout *) tools_->widget(index)->layout();
  if (!layout)
  {
    layout = new QVBoxLayout();
    tools_->widget(index)->setLayout(layout);
  }

  QToolButton * tool = new QToolButton();
  tool->setText(name);
  tool->setToolTip(path);
  tool->setCheckable(true);
  buttons_.push_back(tool);
  connect(tool, SIGNAL(clicked()), this, SLOT(createItem()));

  layout->addWidget(tool);

  tools_->setCurrentIndex(index);
}
