#include "VertexItem.h"
#include "EdgeItem.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>

VertexItem::VertexItem(Vertex* vertex, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-Radius, -Radius, 2 * Radius, 2 * Radius, parent),
      m_vertex(vertex)
{
    m_defaultColor = Qt::cyan;
    setPen(QPen(Qt::black, 2));
    setBrush(m_defaultColor);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);

    setCacheMode(DeviceCoordinateCache);
    setZValue(1);

    m_label = new QGraphicsTextItem(QString::number(m_vertex->getId()), this);
    QFont font = m_label->font();
    font.setPointSize(10);
    font.setBold(true);
    m_label->setFont(font);

    QRectF textRect = m_label->boundingRect();
    m_label->setPos(-textRect.width() / 2, -textRect.height() / 2);

    m_distanceLabel = new QGraphicsTextItem(this);
    QFont distFont = m_label->font();
    distFont.setPointSize(8);
    distFont.setBold(false);
    m_distanceLabel->setFont(distFont);
    m_distanceLabel->setDefaultTextColor(Qt::darkBlue);
    m_distanceLabel->setVisible(false);
}

VertexItem::~VertexItem() {}

void VertexItem::setColor(const QColor& color) {
    setBrush(QBrush(color));
}

void VertexItem::resetColor() {
    setBrush(QBrush(m_defaultColor));
}

void VertexItem::setDistanceText(const QString& text) {
    m_distanceLabel->setHtml(text);
    QRectF distRect = m_distanceLabel->boundingRect();
    m_distanceLabel->setPos(-distRect.width() / 2, Radius + 2);
    m_distanceLabel->setVisible(true);
}

void VertexItem::hideDistanceText() {
    m_distanceLabel->setVisible(false);
}


void VertexItem::addEdge(EdgeItem* edge) {
    if (edge && !m_edgeList.contains(edge)) {
        m_edgeList.append(edge);
    }
}

void VertexItem::removeEdge(EdgeItem* edge) {
    m_edgeList.removeAll(edge);
}

void VertexItem::removeEdges() {
    QList<EdgeItem*> edgesToDelete = m_edgeList;
    m_edgeList.clear();

    for (EdgeItem* edge : edgesToDelete) {
        if (edge->getSourceItem() && edge->getSourceItem() != this) {
            edge->getSourceItem()->removeEdge(edge);
        }
        if (edge->getDestItem() && edge->getDestItem() != this) {
            edge->getDestItem()->removeEdge(edge);
        }

        if (scene()) {
            scene()->removeItem(edge);
        }
        delete edge;
    }
}

QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged && scene()) {
        for (EdgeItem* edge : std::as_const(m_edgeList)) {
            edge->trackVertices();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}