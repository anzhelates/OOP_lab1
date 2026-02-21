#pragma once

#include <QGraphicsPathItem>
#include "Edge.h"

class VertexItem;
class QGraphicsTextItem;

class EdgeItem : public QGraphicsPathItem {
public:
    EdgeItem(Edge* edge, VertexItem* sourceItem, VertexItem* destItem,
             bool isDirected, bool isWeighted, QGraphicsItem* parent = nullptr);
    ~EdgeItem() override;

    Edge* getEdge() const { return m_edge; }
    VertexItem* getSourceItem() const { return m_sourceItem; }
    VertexItem* getDestItem() const { return m_destItem; }

    void setColor(const QColor& color);
    void resetColor();

    void trackVertices();
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    Edge* m_edge;
    VertexItem* m_sourceItem;
    VertexItem* m_destItem;

    bool m_isDirected;
    bool m_isWeighted;

    QColor m_defaultColor;
    QGraphicsTextItem* m_weightLabel;
    QColor m_currentColor;
};