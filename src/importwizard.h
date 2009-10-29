#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include <QtCore/QVector>
#include <QtGui/QtGui>


class ImportWizard : public QWizard
{
  Q_OBJECT

public:
  ImportWizard(QWidget * = 0, Qt::WindowFlags = 0);
  ~ImportWizard();

  static void addFinalText(const QString &);

public slots:
  void browseFiles();
  void clear();
  void check(int);

signals:
  void createToolButton(const QString &, const QString &, const QString &);

private:
  void createInfoPage();
  void createConfigurationPage();
  void createFinalPage();
  void createErrorPage();

private:
  QVector<QWizardPage *> pages_;

  QLineEdit * nameEdit_;
  QLineEdit * categoryEdit_;
  QLineEdit * pathEdit_;

  QTextEdit * finalText_;
  static QStringList final_;

};

#endif /* IMPORTWIZARD_H */
