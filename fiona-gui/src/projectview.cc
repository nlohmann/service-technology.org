#include "projectview.h"
#include <QtCore/QList>


ProjectView::ProjectView(QWidget * parent) :
  QGraphicsView(parent)
{
  setDragMode(RubberBandDrag);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  sourceLink_ = NULL;
  targetLink_ = NULL;
}


ProjectView::ProjectView(QGraphicsScene * scene, QWidget * parent) :
  QGraphicsView(scene, parent)
{
}


ProjectView::~ProjectView()
{
}


Node * ProjectView::addNode()
{
  Node * n = new Node();
  n->setText("unnamed node");
  nodes_.insert(n);
  scene()->addItem(n);
  scene()->clearSelection();
  n->setSelected(true);

  return n;
}


void ProjectView::addLink()
{
  linking();
}


void ProjectView::revertLink()
{
  QList<QGraphicsItem *> selection = scene()->selectedItems();
  if (selection.isEmpty())
    return;
  foreach (QGraphicsItem * link, selection)
  {
    Link * l = dynamic_cast<Link *>(link);
    if (!l)
      continue;
    l->switchNodes();
    break;
  }
}


void ProjectView::linking()
{
  QList<QGraphicsItem *> selection = scene()->selectedItems();
  if (selection.isEmpty())
    sourceLink_ = targetLink_ = NULL;
  else
  {
    foreach (Node * n, nodes_)
    {
      if (n->isSelected())
      {
        if (sourceLink_)
          targetLink_ = n;
        else
          sourceLink_ = n;
      }
    }
    Link * l = new Link(sourceLink_, targetLink_);
    scene()->addItem(l);
    sourceLink_ = targetLink_ = NULL;
  }
}


void ProjectView::removeNode(Node * n)
{
  if (nodes_.contains(n))
  {
    nodes_.remove(n);
    delete n;
  }
}


void ProjectView::removeLink(Link * l)
{
  delete l;
}


const QString ProjectView::getMakefile() const
{
  QString result;

  // head
  result += "FILES = $(wildcard *)\n\n";
  result += "all:";
  foreach (Node * n, nodes_)
    result += " " + n->tool()->package();
  result += "\n\n";

  // body
  foreach (Node * n, nodes_)
  {
    n->tool()->computeSubst();
    QStringList succ;
    foreach (Node * s, n->successors())
      succ.push_back(s->tool()->package());
    result += n->tool()->getMakefile(succ);
  }

  // tail
  foreach (Node * n, nodes_)
  {
    result += n->tool()->output() + ": " + n->tool()->input() + "\n";
    result += "\t" + n->tool()->callString() + "\n";
  }

  return result;
}


void ProjectView::mouseDoubleClickEvent(QMouseEvent * event)
{
  QList<QGraphicsItem *> selection = scene()->selectedItems();
  Node * n = dynamic_cast<Node *>(selection.first());
  if (n)
  {
    emit showDockWidget(n->tool());
  }
}


void ProjectView::keyPressEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_Delete)
  {
    QList<QGraphicsItem *> selection = scene()->selectedItems();
    foreach (QGraphicsItem * item, selection)
    {
      Node * n = dynamic_cast<Node *>(item);
      if (n)
      {
        scene()->removeItem(item);
        removeNode(n);
      }
      Link * l = dynamic_cast<Link *>(item);
      if (l)
      {
        scene()->removeItem(item);
        removeLink(l);
      }
    }
  }
}
