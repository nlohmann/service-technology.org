#ifndef NODE_H
#define NODE_H

#include <QtGui/QApplication>
#include <QtGui/QColor>
#include <QtGui/QGraphicsItem>
#include <QtCore/QSet>

class Link;
class Tool;

class Node : public QGraphicsItem
{
    Q_DECLARE_TR_FUNCTIONS(Node)

public:
    Node();
    ~Node();

    void setText(const QString &text);
    QString text() const;
    void setTextColor(const QColor &color);
    QColor textColor() const;
    void setOutlineColor(const QColor &color);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void addLink(Link *link);
    void removeLink(Link *link);

    void addPostNode(Node *);
    void removePostNode(Node *);
    void addPreNode(Node *);
    void removePreNode(Node *);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    Tool * createTool();
    Tool * createTool(const QString &);
    Tool * tool() const;

    QSet<Node *> successors() const
    {
      return preNode_;
    }

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

private:
    QRectF outlineRect() const;
    int roundness(double size) const;

    QSet<Link *> myLinks;
    QSet<Node *> preNode_;
    QSet<Node *> postNode_;
    QString myText;
    QColor myTextColor;
    QColor myBackgroundColor;
    QColor myOutlineColor;

    Tool * tool_;

};

#endif /* NODE_H */
