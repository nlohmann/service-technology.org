#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "link.h"
#include "node.h"
#include "tool.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QKeyEvent>


class ProjectView : public QGraphicsView
{
  Q_OBJECT

public:
  ProjectView(QWidget * = 0);
  ProjectView(QGraphicsScene *, QWidget * = 0);
  ~ProjectView();

  Node * addNode();
  void addLink();
  void revertLink();

  void removeNode(Node *);
  void removeLink(Link *);

  const QString getMakefile() const;

public slots:
  void linking();

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
  void keyPressEvent(QKeyEvent *);

private:
  QSet<Node *> nodes_;

  Node * sourceLink_;
  Node * targetLink_;

signals:
  void showDockWidget(Tool *);

};

#endif /* PROJECTVIEW_H */
