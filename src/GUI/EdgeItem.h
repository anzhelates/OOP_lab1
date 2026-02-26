#pragma once
#include <QGraphicsPathItem>
#include <QPen>

class VertexItem;
class QGraphicsTextItem;

class EdgeItem : public QGraphicsPathItem {
public:
    EdgeItem(int fromId, int toId, VertexItem* source, VertexItem* dest, double weight, bool directed, bool weighted, QGraphicsItem* parent = nullptr);
    ~EdgeItem() override;

    int getFromId() const { return m_fromId; }
    int getToId() const { return m_toId; }

    VertexItem* sourceNode() const { return m_source; }
    VertexItem* destNode() const { return m_dest; }

    void clearSource() { m_source = nullptr; }
    void clearDest() { m_dest = nullptr; }

    void updatePosition();
    void setWeight(double weight);

    void setColor(const QColor& color);
    void resetColor();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int m_fromId;
    int m_toId;
    double m_weight;
    bool m_directed;
    bool m_weighted;

    VertexItem* m_source;
    VertexItem* m_dest;
    QGraphicsTextItem* m_weightText;
    QColor m_defaultColor;
};