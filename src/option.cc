#include "option.h"

#include <QtCore/QStringList>


Option::Option() :
  optionWidget_(NULL), optionLine_(NULL), argType_(NONE), required_(false),
  argoptional_(false), multiple_(false), hidden_(false), flag_(false),
  flagGiven_(false), type_(NON)
{
}


Option::~Option()
{
}


QWidget * Option::widget()
{
  if (optionWidget_)
    return optionWidget_;

  QVBoxLayout * groupLayout = new QVBoxLayout();

  QCheckBox * activator = new QCheckBox(long_);
  optionChecks_.push_back(activator);
  if (flagGiven_)
  {
    if (flag_)
      activator->setChecked(true);
    groupLayout->addWidget(activator);
  }
  else
  {
    if (required_)
    {
      activator->setEnabled(false);
      activator->setChecked(true);
    }
    groupLayout->addWidget(activator);

    if (!typeString_.isEmpty())
    {
      QLabel * typestr = new QLabel(typeString_+":");
      groupLayout->addWidget(typestr);
    }

    if (!values_.isEmpty())
    {
      if (multiple_)
      {
        foreach (QString val, values_)
        {
          QCheckBox * value = new QCheckBox(val);
          if (!required_)
            value->setEnabled(false);
          optionChecks_.push_back(value);
          if (default_ == val)
          {
            if (required_)
              value->setChecked(true);
            else
              connect(activator, SIGNAL(toggled(bool)), value, SLOT(setChecked(bool)));
          }
          connect(activator, SIGNAL(toggled(bool)), value, SLOT(setEnabled(bool)));
          groupLayout->addWidget(value);
        }
      }
      else
      {
        foreach (QString val, values_)
        {
          QRadioButton * value = new QRadioButton(val);
          if (!required_)
            value->setEnabled(false);
          optionChecks_.push_back(value);
          if (default_ == val && required_)
          {
            value->setChecked(true);
          }
          connect(activator, SIGNAL(toggled(bool)), value, SLOT(setEnabled(bool)));
          groupLayout->addWidget(value);
        }
      }
    }
    else
    {
      if (!flagGiven_)
      {
        optionLine_ = new QLineEdit();
        if (!required_)
          optionLine_->setEnabled(false);
        connect(activator, SIGNAL(toggled(bool)), optionLine_, SLOT(setEnabled(bool)));
        groupLayout->addWidget(optionLine_);
      }
    }
  }

  QGroupBox * group = new QGroupBox();
  group->setLayout(groupLayout);
  group->setTitle(long_);
  group->setToolTip(description_);

  QFormLayout * layout = new QFormLayout();
  layout->addWidget(group);

  optionWidget_ = new QWidget();
  optionWidget_->setLayout(layout);

  return optionWidget_;
}


const QString Option::helpString() const
{
  QString result;

  result += "option \""+long_+"\" "+short_+" \""+description_+"\"\n";
  if (!details_.isEmpty())
    result += "  details=\""+details_+"\"\n";
  if (argType_ != NONE)
    result += "  argtype\n";
  if (!typeString_.isEmpty())
    result += "  typestr=\""+typeString_+"\"\n";
  if (!values_.isEmpty())
  {
    result += "  values=";
    foreach (QString val, values_)
      result += "\""+val+"\" ";
    result += "\n";
  }
  if (!default_.isEmpty())
    result += "  default=\""+default_+"\"\n";
  if (!dependon_.isEmpty())
    result += "  dependon=\""+dependon_+"\"\n";
  if (required_)
    result += "  required\n";
  else
    result += "  optional\n";
  if (argoptional_)
    result += "  argoptional\n";
  if (multiple_)
    result += "  multiple\n";
  if (hidden_)
    result += "  hidden\n";

  return result;
}


const QString Option::callString() const
{
  QString result;
  if (!isChecked())
    return result;
  switch (type_)
  {
  case NON:
    {
      for (int i = 1; i < optionChecks_.size(); i++)
        if (optionChecks_[i]->isChecked())
          result += "--" + long_ + "=" + optionChecks_[i]->text() + " ";
      break;
    }
  case INPUT:
    {
      result += "--" + long_ + "=$< ";
      break;
    }
  case OUTPUT:
    {
      result += "--" + long_ + "=$@ ";
      break;
    }
  default:
    break;
  }

  return result;
}


bool Option::isChecked() const
{
  if (optionChecks_.isEmpty())
    return false;
  return optionChecks_[0]->isChecked();
}
