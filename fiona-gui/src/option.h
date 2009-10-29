#ifndef OPTION_H
#define OPTION_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QtGui>


enum ArgType
{
  NONE, STRING, INT, SHORT, LONG, FLOAT, DOUBLE, LONGDOUBLE, LONGLONG
};

enum OptionType
{
  NON, INPUT, OUTPUT
};


class Option : public QObject
{
  Q_OBJECT

public:
  Option();
  ~Option();

  QWidget * widget();
  const QString callString() const;

  const QString helpString() const;

  bool isChecked() const;

private:
  QWidget * optionWidget_;
  QVector<QAbstractButton *> optionChecks_;

public:
  QString long_;
  QString short_;
  QString description_;
  QString details_;
  ArgType argType_;
  QString typeString_;
  QVector<QString> values_;
  QString default_;
  QString dependon_;
  bool required_;
  bool argoptional_;
  bool multiple_;
  bool hidden_; // obsolete

  bool flag_; // 1: on 0: off
  bool flagGiven_;

  OptionType type_;

  QLineEdit * optionLine_;

};

#endif /* OPTION_H */
