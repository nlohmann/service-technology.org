#include <QtGui/QtGui>

#include "link.h"
#include "node.h"


const float Pi = 3.14;


Link::Link(Node * fromNode, Node * toNode)
{
  fromNode_ = fromNode;
  toNode_ = toNode;

  fromNode_->addLink(this);
  fromNode_->addPostNode(toNode_);
  toNode_->addLink(this);
  toNode_->addPreNode(fromNode_);

  setFlags(QGraphicsItem::ItemIsSelectable);
  setZValue(1);

  setColor(Qt::darkRed);
  pen().setBrush(Qt::darkRed);
  trackNodes();
  setSelected(true);
}


Link::~Link()
{
  fromNode_->removeLink(this);
  fromNode_->removePostNode(toNode_);
  toNode_->removeLink(this);
  toNode_->removePreNode(fromNode_);
}


Node * Link::fromNode() const
{
  return fromNode_;
}


Node * Link::toNode() const
{
  return toNode_;
}


void Link::switchNodes()
{
  Node * tmp = fromNode_;
  fromNode_ = toNode_;
  toNode_ = tmp;
}


void Link::setColor(const QColor & color)
{
  setPen(QPen(color, 1.0));
}


QColor Link::color() const
{
  return pen().color();
}


void Link::trackNodes()
{
  QLineF line(mapFromItem(fromNode_, 0, 0), mapFromItem(toNode_, 0, 0));
  setLine(line);
}


void Link::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * parent)
{
  if (fromNode_->collidesWithItem(toNode_))
    return;

  float arrowSize = 20;
  painter->setPen(color());
  painter->setBrush(color());

  double angle = acos(line().dx() / line().length());
  if (line().dy() >= 0)
  {
    angle = (Pi * 2) - angle;
    QPointF arrowP1 = line().p2() - QPointF(sin(angle + Pi / 3) * arrowSize, cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p2() - QPointF(sin(angle + Pi - Pi / 3) * arrowSize, cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead_.clear();
    arrowHead_ << line().p2() << arrowP1 << line().p2() << arrowP2;
    painter->drawLine(line());
    painter->drawPolyline(arrowHead_);
  }
  else
  {
    angle = (Pi * 2) + angle;
    QPointF arrowP1 = line().p2() - QPointF(sin(angle + Pi / 3) * arrowSize, cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p2() - QPointF(sin(angle + Pi - Pi / 3) * arrowSize, cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead_.clear();
    arrowHead_ << line().p2() << arrowP1 << line().p2() << arrowP2;
    painter->drawLine(line());
    painter->drawPolyline(arrowHead_);
  }
}
