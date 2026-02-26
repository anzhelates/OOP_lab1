#pragma once
#include <QGraphicsEllipseItem>
#include <QColor>
#include <QString>
#include <QList>

class QGraphicsTextItem;
class EdgeItem;

class VertexItem : public QGraphicsEllipseItem {
public:
    explicit VertexItem(int id, const QString& label, QGraphicsItem* parent = nullptr);
    ~VertexItem() override;

    int getId() const { return m_id; }
    void setLabel(const QString& label);

    void setColor(const QColor& color);
    void resetColor();
    void setDistanceText(const QString& text);
    void hideDistanceText();

    void addEdge(EdgeItem* edge);
    void removeEdge(EdgeItem* edge);
    QList<EdgeItem*> getEdges() const { return m_edges; }

    static constexpr int Radius = 20;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int m_id;
    QColor m_defaultColor;
    QGraphicsTextItem* m_labelItem;
    QGraphicsTextItem* m_distItem;
    QList<EdgeItem*> m_edges;
};