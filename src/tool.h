#ifndef TOOL_H
#define TOOL_H


#include <QtCore/QtCore>
#include <QtGui/QtGui>


class Option;


class Tool : public QObject
{
  Q_OBJECT

public:
  Tool();
  Tool(const QString &);
  ~Tool();

  void setPackage(const QString &);
  void setVersion(const QString &);
  void setPurpose(const QString &);
  void setUsage(const QString &);
  void setDescription(const QString &);

  void setText(const QString &);

  void addOption(Option *); //< adds an option to default section
  void addOption(const QString &, Option *); //< adds an option to specified section

  void mapSection(const QString &, const QString &);
  QString package() const
  {
    return package_;
  }
  QString purpose() const
  {
    return purpose_;
  }
  int parse(const QString &);
  QString callString() const;
  QWidget * dockWidgetContents();

  const QString getMakefile(QStringList) const;
  void computeSubst();

  const QString helpString() const;

  const QString input() const { return input_; }
  const QString output() const { return output_; }

private:
  QString package_;
  QString version_;
  QString purpose_;
  QString usage_;
  QString description_;

  QVector<QSet<Option *> > options_;
  QVector<QString> sections_;
  QMap<QString, QString> sectionDesc_;

  QString text_;

  QWidget * dockContents_;

  QString input_;
  QString output_;

};

#endif /* TOOL_H */
