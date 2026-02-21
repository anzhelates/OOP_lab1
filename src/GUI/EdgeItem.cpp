#include "EdgeItem.h"
#include "VertexItem.h"
#include <QPainter>
#include <QPen>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
#include <QtMath>
#include <QDebug>

EdgeItem::EdgeItem(Edge* edge, VertexItem* sourceItem, VertexItem* destItem,
                   bool isDirected, bool isWeighted, QGraphicsItem* parent)
    : QGraphicsPathItem(parent),
      m_edge(edge),
      m_sourceItem(sourceItem),
      m_destItem(destItem),
      m_isDirected(isDirected),
      m_isWeighted(isWeighted),
      m_weightLabel(nullptr)
{
    m_defaultColor = Qt::black;
    setPen(QPen(m_defaultColor, 2));
    setZValue(0);

    setFlag(ItemIsSelectable);

    if (m_isWeighted) {
        m_weightLabel = new QGraphicsTextItem(QString::number(m_edge->getWeight()), this);
        m_weightLabel->setDefaultTextColor(Qt::blue);
        m_weightLabel->setFont(QFont("Arial", 8));
    }

    m_sourceItem->addEdge(this);
    m_destItem->addEdge(this);

    trackVertices();
}

EdgeItem::~EdgeItem() {
    if (m_sourceItem) m_sourceItem->removeEdge(this);
    if (m_destItem) m_destItem->removeEdge(this);
}

void EdgeItem::setColor(const QColor& color) {
    setPen(QPen(color, 2));
    if (m_weightLabel && color != m_defaultColor) {
        m_weightLabel->setDefaultTextColor(color);
    }
    update();
}

void EdgeItem::resetColor() {
    setPen(QPen(m_defaultColor, 2));
    if (m_weightLabel) {
        m_weightLabel->setDefaultTextColor(Qt::blue);
    }
    update();
}

void EdgeItem::trackVertices() {
    if (!m_sourceItem || !m_destItem) return;

    prepareGeometryChange();

    QPointF p1 = m_sourceItem->pos();
    QPointF p2 = m_destItem->pos();

    QPainterPath newPath;
    newPath.moveTo(p1);
    newPath.lineTo(p2);
    setPath(newPath);

    if (m_weightLabel) {
        QLineF centerLine(p1, p2);
        QPointF unitVector(0,0);
        if(centerLine.length() > 0) {
             unitVector = centerLine.unitVector().p2() - centerLine.unitVector().p1();
        }
        QPointF offsetStart = p1 + unitVector * VertexItem::Radius;
        QPointF offsetEnd = p2 - unitVector * VertexItem::Radius;

        QPointF center = (offsetStart + offsetEnd) / 2;
        center += QPointF(3, -3);
        m_weightLabel->setPos(center);
    }
    update();
}

QRectF EdgeItem::boundingRect() const {
    qreal extra = pen().widthF() + 10;
    return path().boundingRect().adjusted(-extra, -extra, extra, extra);
}

QPainterPath EdgeItem::shape() const {
    QPainterPathStroker stroker;
    stroker.setWidth(10.0);
    return stroker.createStroke(path());
}

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(widget);
    if (!m_sourceItem || !m_destItem) return;
    QPen linePen = (option->state & QStyle::State_Selected)
        ? QPen(Qt::magenta, 3, Qt::DashLine)
        : pen();

    painter->setPen(linePen);

    QLineF centerLine(m_sourceItem->pos(), m_destItem->pos());
    if (centerLine.length() == 0) return;

    const qreal radius = VertexItem::Radius;
    QPointF unitVector = centerLine.unitVector().p2() - centerLine.unitVector().p1();
    QPointF offsetStart = centerLine.p1() + unitVector * radius;
    QPointF offsetEnd   = centerLine.p2() - unitVector * radius;

    QLineF visibleLine(offsetStart, offsetEnd);

    if (visibleLine.length() <= 0) return;
    painter->drawLine(visibleLine);

    if (m_isDirected) {
        const qreal arrowSize = 12.0;
        qreal angle = std::atan2(visibleLine.dy(), visibleLine.dx());
        QPointF arrowP1 = offsetEnd - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                              std::sin(angle + M_PI / 6) * arrowSize);
        QPointF arrowP2 = offsetEnd - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                              std::sin(angle - M_PI / 6) * arrowSize);
        QPolygonF arrowHead;
        arrowHead << offsetEnd << arrowP1 << arrowP2;
        painter->setBrush(linePen.color());
        painter->drawPolygon(arrowHead);
    }
}