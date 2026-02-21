#pragma once

#include <QGraphicsEllipseItem>
#include <QColor>
#include <QList>
#include <QVariant>
#include "Vertex.h"

class QGraphicsTextItem;
class EdgeItem;

class VertexItem : public QGraphicsEllipseItem {
public:
    explicit VertexItem(Vertex* vertex, QGraphicsItem* parent = nullptr);
    ~VertexItem() override;

    Vertex* getVertex() const { return m_vertex; }
    int getId() const { return m_vertex ? m_vertex->getId() : -1; }

    void setColor(const QColor& color);
    void resetColor();

    void addEdge(EdgeItem* edge);
    void removeEdge(EdgeItem* edge);
    void removeEdges();

    void setDistanceText(const QString& text);
    void hideDistanceText();

    static constexpr int Radius = 15;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    Vertex* m_vertex;
    QGraphicsTextItem* m_label;
    QGraphicsTextItem* m_distanceLabel;
    QColor m_defaultColor;
    QList<EdgeItem*> m_edgeList;
};