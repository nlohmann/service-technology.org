#ifndef LINK_H
#define LINK_H

#include <QtGui/QGraphicsLineItem>

class Node;

class Link : public QGraphicsLineItem
{
public:
  Link(Node *, Node *);
  ~Link();

  Node * fromNode() const;
  Node * toNode() const;

  void switchNodes();

  void setColor(const QColor &);
  QColor color() const;

  void trackNodes();

protected:
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0);

private:
  Node * fromNode_;
  Node * toNode_;

  QPolygonF arrowHead_;

};

#endif /* LINK_H */
