#include "option.h"
#include "tool.h"

#include <iostream>
#include <QtGui/QtGui>
#include <string>

// Global variables needed by Tool parser
Tool * t;
extern FILE * yyin;
extern int yyparse();


Tool::Tool() :
  QObject()
{
  sections_.push_back("default");
  options_.push_back(QSet<Option *>());

  dockContents_ = NULL;
}


Tool::Tool(const QString & path) :
  QObject()
{
  sections_.push_back("default");
  options_.push_back(QSet<Option *>());
  dockContents_ = NULL;

  parse(path);
}


Tool::~Tool()
{
}


int Tool::parse(const QString & path)
{
  t = this;
  yyin = fopen(path.toStdString().c_str(), "r");
  int res = yyparse();
  fclose(yyin);

  /*if (!res)
    std::cout << helpString().toStdString();*/

  return res;
}


void Tool::setPackage(const QString & package)
{
  package_ = package;
}


void Tool::setVersion(const QString & version)
{
  version_ = version;
}


void Tool::setPurpose(const QString & purpose)
{
  purpose_ = purpose;
}


void Tool::setUsage(const QString & usage)
{
  usage_ = usage;
}


void Tool::setDescription(const QString & description)
{
  description_ = description;
}


void Tool::setText(const QString & text)
{
  text_ = text;
}


void Tool::addOption(Option * o)
{
  options_[0].insert(o);
}


void Tool::addOption(const QString & sect, Option * o)
{
  int i;
  for (i = 0; i < sections_.size(); i++)
    if (sections_[i] == sect)
      break;
  if (i < sections_.size())
    options_[i].insert(o);
  else
  {
    sections_.push_back(sect);
    options_.push_back(QSet<Option *>());
    options_[i].insert(o);
  }
}


void Tool::mapSection(const QString & sect, const QString & desc)
{
  sectionDesc_[sect] = desc;
}


QString Tool::callString() const
{
  QString result = package_ + " ";
  for (int i = 0; i < options_.size(); i++)
    for (QSet<Option *>::const_iterator o = options_[i].begin(); o != options_[i].end(); o++)
      result += (*o)->callString() + " ";

  return result;
}


QWidget * Tool::dockWidgetContents()
{
  if (dockContents_)
    return dockContents_;

  QVBoxLayout * layout = new QVBoxLayout();

  for (int i = 0; i < sections_.size(); i++)
  {
    if (options_[i].isEmpty())
      continue;
    QGroupBox * group = new QGroupBox(sections_[i]);
    group->setToolTip(sectionDesc_[sections_[i]]);
    QFormLayout * groupLayout = new QFormLayout();
    foreach (Option * o, options_[i])
    {
      groupLayout->addWidget(o->widget());
    }
    group->setLayout(groupLayout);
    layout->addWidget(group);
  }

  dockContents_ = new QWidget();
  dockContents_->setLayout(layout);

  return dockContents_;
}


const QString Tool::helpString() const
{
  QString result;

  if (!package_.isEmpty())
    result += "package \""+package_+"\"\n";
  if (!version_.isEmpty())
    result += "version \""+version_+"\"\n";
  if (!purpose_.isEmpty())
    result += "purpose \""+purpose_+"\"\n";
  if (!usage_.isEmpty())
    result += "usage \""+usage_+"\"\n";
  if (!description_.isEmpty())
    result += "description \""+description_+"\"\n";
  result+="\n";

  for (int i = 0; i < sections_.size(); i++)
  {
    if (i != 0)
      result += "section \""+sections_[i]+"\"";
    if (sectionDesc_.count(sections_[i]))
      result += " sectiondesc=\""+sectionDesc_[sections_[i]]+"\"";
    result += "\n";
    foreach (Option * o, options_[i])
    {
      result += o->helpString();
      result += "\n";
    }
  }

  if (!text_.isEmpty())
    result += "text \""+text_+"\"\n";

  return result;
}


const QString Tool::getMakefile(QStringList successors) const
{
  QString result;
  QString input = input_;
  QString output = output_;

  result += package_ + ": $(FILES:" + input.remove('%') + "=" + output.remove('%') + ")\n";
  foreach (QString successor, successors)
    result += "\t$(MAKE) " + successor + "\n";

  result += "\n";

  return result;
}


void Tool::computeSubst()
{
  for (int i = 0; i < options_.size(); i++)
    for (QSet<Option *>::const_iterator o = options_[i].begin(); o != options_[i].end(); o++)
    {
      if (!(*o)->isChecked())
        continue;
      if ((*o)->type_ == INPUT)
        input_ = (*o)->optionLine_->text();
      if ((*o)->type_ == OUTPUT)
        output_ = (*o)->optionLine_->text();
    }
}
