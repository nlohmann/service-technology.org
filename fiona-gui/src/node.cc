#include <QtGui/QtGui>

#include "link.h"
#include "node.h"
#include "tool.h"

Node::Node()
{
  myTextColor = Qt::darkGreen;
  myOutlineColor = Qt::darkBlue;
  myBackgroundColor = Qt::white;

  setFlags(ItemIsMovable | ItemIsSelectable);
}


Node::~Node()
{
  foreach (Link *link, myLinks)
    delete link;
  foreach (Node * n, postNode_)
    n->removePreNode(this);
  foreach (Node * n, preNode_)
    n->removePostNode(this);
}


void Node::setText(const QString &text)
{
  prepareGeometryChange();
  myText = text;
  update();
}


QString Node::text() const
{
  return myText;
}


void Node::setTextColor(const QColor &color)
{
  myTextColor = color;
  update();
}


QColor Node::textColor() const
{
  return myTextColor;
}


void Node::setOutlineColor(const QColor &color)
{
  myOutlineColor = color;
  update();
}


QColor Node::outlineColor() const
{
  return myOutlineColor;
}


void Node::setBackgroundColor(const QColor &color)
{
  myBackgroundColor = color;
  update();
}


QColor Node::backgroundColor() const
{
  return myBackgroundColor;
}


void Node::addLink(Link *link)
{
  myLinks.insert(link);
}


void Node::removeLink(Link *link)
{
  myLinks.remove(link);
}


void Node::addPostNode(Node * node)
{
  postNode_.insert(node);
}


void Node::removePostNode(Node * node)
{
  postNode_.remove(node);
}


void Node::addPreNode(Node * node)
{
  preNode_.insert(node);
}


void Node::removePreNode(Node * node)
{
  preNode_.remove(node);
}


QRectF Node::boundingRect() const
{
  const int Margin = 1;
  return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}


QPainterPath Node::shape() const
{
  QRectF rect = outlineRect();

  QPainterPath path;
  path.addRoundRect(rect, roundness(rect.width()), roundness(rect.height()));
  return path;
}


void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
  QPen pen(myOutlineColor);
  if (option->state & QStyle::State_Selected)
  {
    pen.setStyle(Qt::DotLine);
    pen.setWidth(2);
  }
  painter->setPen(pen);
  painter->setBrush(myBackgroundColor);

  QRectF rect = outlineRect();
  painter->drawRoundRect(rect, roundness(rect.width()),
      roundness(rect.height()));

  painter->setPen(myTextColor);
  painter->drawText(rect, Qt::AlignCenter, myText);
}


QVariant Node::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
  if (change == ItemPositionHasChanged)
  {
    foreach (Link *link, myLinks)
        link->trackNodes();
  }
  return QGraphicsItem::itemChange(change, value);
}


QRectF Node::outlineRect() const
{
  const int Padding = 8;
  QFontMetricsF metrics = qApp->font();
  QRectF rect = metrics.boundingRect(myText);
  rect.adjust(-Padding, -Padding, +Padding, +Padding);
  rect.translate(-rect.center());
  return rect;
}

int Node::roundness(double size) const
{
  const int Diameter = 12;
  return 100 * Diameter / int(size);
}

Tool * Node::createTool()
{
  tool_ = new Tool();
  return tool_;
}


Tool * Node::createTool(const QString & path)
{
  tool_ = new Tool(path);
  return tool_;
}


Tool * Node::tool() const
{
  return tool_;
}
