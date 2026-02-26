/**
 * @file VertexItem.cpp
 * @brief Implementation of the VertexItem class.
 */
#include "VertexItem.h"
#include "EdgeItem.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>

VertexItem::VertexItem(int id, const QString& label, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-Radius, -Radius, 2 * Radius, 2 * Radius, parent),
      m_id(id)
{
    m_defaultColor = Qt::cyan;
    setPen(QPen(Qt::black, 2));
    setBrush(m_defaultColor);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setZValue(1);

    m_labelItem = new QGraphicsTextItem(label, this);
    QFont font = m_labelItem->font();
    font.setPointSize(10);
    font.setBold(true);
    m_labelItem->setFont(font);

    QRectF textRect = m_labelItem->boundingRect();
    m_labelItem->setPos(-textRect.width() / 2, -textRect.height() / 2);

    m_distItem = new QGraphicsTextItem(this);
    QFont distFont = m_labelItem->font();
    distFont.setPointSize(8);
    distFont.setBold(false);
    m_distItem->setFont(distFont);
    m_distItem->setDefaultTextColor(Qt::darkBlue);
    m_distItem->setVisible(false);
}

VertexItem::~VertexItem() {
    for (EdgeItem* edge : std::as_const(m_edges)) {
        if (edge->sourceNode() == this) edge->clearSource();
        if (edge->destNode() == this) edge->clearDest();
    }
}

void VertexItem::setLabel(const QString& label) {
    m_labelItem->setPlainText(label);
    QRectF textRect = m_labelItem->boundingRect();
    m_labelItem->setPos(-textRect.width() / 2, -textRect.height() / 2);
}

void VertexItem::setColor(const QColor& color) { setBrush(QBrush(color)); }
void VertexItem::resetColor() { setBrush(QBrush(m_defaultColor)); }

void VertexItem::setDistanceText(const QString& text) {
    m_distItem->setHtml(text);
    QRectF distRect = m_distItem->boundingRect();
    m_distItem->setPos(-distRect.width() / 2, Radius + 2);
    m_distItem->setVisible(true);
}

void VertexItem::hideDistanceText() { m_distItem->setVisible(false); }

void VertexItem::addEdge(EdgeItem* edge) {
    if (edge && !m_edges.contains(edge)) m_edges.append(edge);
}

void VertexItem::removeEdge(EdgeItem* edge) {
    m_edges.removeAll(edge);
}

QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged && scene()) {
        for (EdgeItem* edge : std::as_const(m_edges)) {
            edge->updatePosition();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}