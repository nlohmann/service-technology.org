#include "importwizard.h"
#include "tool.h"


QStringList ImportWizard::final_;


ImportWizard::ImportWizard(QWidget * parent, Qt::WindowFlags flags) :
  QWizard(parent, flags)
{
  createInfoPage();
  createConfigurationPage();
  createFinalPage();

  addPage(pages_[0]);
  addPage(pages_[1]);
  addPage(pages_[2]);

  connect(this, SIGNAL(accepted()), this, SLOT(clear()));
  connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(check(int)));

  setOption(QWizard::NoCancelButton);
}


ImportWizard::~ImportWizard()
{
  foreach (QWizardPage * page, pages_)
    delete page;
  pages_.clear();
}


void ImportWizard::createInfoPage()
{
  QWizardPage * page = new QWizardPage();

  page->setTitle(tr("Tool Import Wizard"));

  QLabel * text = new QLabel(tr("This wizard is going to lead you through the "
      "process of importing a new tool into <i>Fiona</i>. To add a new tool to "
      "your workspace's tool box you need a gengetopt input file and an idea "
      "in which category the new tool fits (Later it should be possible to "
      "dynamically extend the tool box)."));
  text->setWordWrap(true);

  QFormLayout * layout = new QFormLayout();
  layout->addWidget(text);

  page->setLayout(layout);

  pages_.push_back(page);
}


void ImportWizard::createConfigurationPage()
{
  QWizardPage * page = new QWizardPage();

  page->setTitle(tr("Tool Import Wizard"));
  page->setSubTitle(tr("Specify the gengetopt input file, a name, and a "
      "label for the tool box's button."));

  QLabel * nameLabel = new QLabel(tr("Name:"));
  nameEdit_ = new QLineEdit();
  QLabel * categoryLabel = new QLabel(tr("Category:"));
  categoryEdit_ = new QLineEdit();
  QLabel * pathLabel = new QLabel(tr("Path:"));
  pathEdit_ = new QLineEdit();
  QPushButton * pathButton = new QPushButton(tr("Browse"));

  connect(pathButton, SIGNAL(clicked()), this, SLOT(browseFiles()));

  QGridLayout * layout = new QGridLayout();
  layout->addWidget(nameLabel, 0, 0);
  layout->addWidget(nameEdit_, 0, 1, 1, 2);
  layout->addWidget(categoryLabel, 1, 0);
  layout->addWidget(categoryEdit_, 1, 1, 1, 2);
  layout->addWidget(pathLabel, 2, 0);
  layout->addWidget(pathEdit_, 2, 1);
  layout->addWidget(pathButton, 2, 2);

  page->setLayout(layout);

  pages_.push_back(page);
}


void ImportWizard::createFinalPage()
{
  QWizardPage * page = new QWizardPage();

  page->setTitle(tr("Final Step"));

  finalText_ = new QTextEdit();

  QFormLayout * layout = new QFormLayout();
  layout->addWidget(finalText_);

  page->setLayout(layout);

  pages_.push_back(page);
}


void ImportWizard::browseFiles()
{
  QString filepath = QFileDialog::getOpenFileName(this,
      tr("Open Gengetopt file"), ".", tr("Gengetopt file (*.ggo)"));

  pathEdit_->setText(filepath);
}


void ImportWizard::clear()
{
  if (finalText_->toPlainText().startsWith(QString("--SUCCESS--")))
    emit createToolButton(nameEdit_->text(), categoryEdit_->text(), pathEdit_->text());

  nameEdit_->clear();
  categoryEdit_->clear();
  pathEdit_->clear();
}


void ImportWizard::check(int index)
{
  switch (index)
  {
  case 2:
  {
    if (nameEdit_->text().isEmpty() || categoryEdit_->text().isEmpty() ||
        pathEdit_->text().isEmpty())
    {
      back();
      QMessageBox::critical(this, tr("An input error occured"),
          tr("Please fill all fields."));
      break;
    }
    if (!QFile::exists(pathEdit_->text()))
    {
      back();
      QMessageBox::critical(this, tr("Input file error"),
          "The file "+pathEdit_->text()+" does not exist!");
      break;
    }
    Tool * t = new Tool();
    if (!t->parse(pathEdit_->text()))
    {
      QString final = "--SUCCESS--";
      foreach (QString f, final_)
      {
        final += '\n';
        final += f;
      }
      finalText_->setText(final);
    }
    else
    {
      QString final = "--ERROR--";
      foreach (QString f, final_)
      {
        final += '\n';
        final += f;
      }
      finalText_->setText(final);
    }
    final_.clear();
    delete t;
    break;
  }
  default:
    break;
  }
}


void ImportWizard::addFinalText(const QString & text)
{
  final_.push_back(text);
}
