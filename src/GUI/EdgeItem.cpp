/**
 * @file EdgeItem.cpp
 * @brief Implementation of the EdgeItem class representing a graphical graph edge.
 */
#include "EdgeItem.h"
#include "VertexItem.h"
#include <QPainter>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
#include <QtMath>

EdgeItem::EdgeItem(int fromId, int toId, VertexItem* source, VertexItem* dest,
                   double weight, bool directed, bool weighted, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_fromId(fromId), m_toId(toId),
      m_weight(weight), m_directed(directed), m_weighted(weighted),
      m_source(source), m_dest(dest), m_weightText(nullptr)
{
    m_defaultColor = Qt::black;
    setPen(QPen(m_defaultColor, 2));
    setZValue(0);
    setFlag(ItemIsSelectable);

    if (m_weighted) {
        m_weightText = new QGraphicsTextItem(QString::number(m_weight, 'f', 1), this);
        m_weightText->setDefaultTextColor(Qt::blue);
        m_weightText->setFont(QFont("Arial", 8));
    }

    if (m_source) m_source->addEdge(this);
    if (m_dest) m_dest->addEdge(this);
    updatePosition();
}

EdgeItem::~EdgeItem() {
    if (m_source) m_source->removeEdge(this);
    if (m_dest) m_dest->removeEdge(this);
}

void EdgeItem::setColor(const QColor& color) {
    setPen(QPen(color, 2));
    if (m_weightText && color != m_defaultColor) m_weightText->setDefaultTextColor(color);
    update();
}

void EdgeItem::resetColor() {
    setPen(QPen(m_defaultColor, 2));
    if (m_weightText) m_weightText->setDefaultTextColor(Qt::blue);
    update();
}

void EdgeItem::setWeight(double weight) {
    m_weight = weight;
    if (m_weightText) {
        m_weightText->setPlainText(QString::number(m_weight, 'f', 1));
    }
}

void EdgeItem::updatePosition() {
    if (!m_source || !m_dest) return;
    prepareGeometryChange();

    QPointF p1 = m_source->pos();
    QPointF p2 = m_dest->pos();

    QPainterPath newPath;
    newPath.moveTo(p1);
    newPath.lineTo(p2);
    setPath(newPath);

    if (m_weightText) {
        QLineF centerLine(p1, p2);
        QPointF unitVector(0,0);
        if(centerLine.length() > 0) unitVector = centerLine.unitVector().p2() - centerLine.unitVector().p1();

        QPointF offsetStart = p1 + unitVector * VertexItem::Radius;
        QPointF offsetEnd = p2 - unitVector * VertexItem::Radius;
        QPointF center = (offsetStart + offsetEnd) / 2;
        center += QPointF(3, -3);
        m_weightText->setPos(center);
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

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    if (!m_source || !m_dest) return;
    QPen linePen = (option->state & QStyle::State_Selected) ? QPen(Qt::magenta, 3, Qt::DashLine) : pen();
    painter->setPen(linePen);

    QLineF centerLine(m_source->pos(), m_dest->pos());
    if (centerLine.length() == 0) return;

    QPointF unitVector = centerLine.unitVector().p2() - centerLine.unitVector().p1();
    QPointF offsetStart = centerLine.p1() + unitVector * VertexItem::Radius;
    QPointF offsetEnd   = centerLine.p2() - unitVector * VertexItem::Radius;
    QLineF visibleLine(offsetStart, offsetEnd);

    if (visibleLine.length() <= 0) return;
    painter->drawLine(visibleLine);

    if (m_directed) {
        const qreal arrowSize = 12.0;
        qreal angle = std::atan2(visibleLine.dy(), visibleLine.dx());
        QPointF arrowP1 = offsetEnd - QPointF(std::cos(angle + M_PI / 6) * arrowSize, std::sin(angle + M_PI / 6) * arrowSize);
        QPointF arrowP2 = offsetEnd - QPointF(std::cos(angle - M_PI / 6) * arrowSize, std::sin(angle - M_PI / 6) * arrowSize);
        QPolygonF arrowHead;
        arrowHead << offsetEnd << arrowP1 << arrowP2;
        painter->setBrush(linePen.color());
        painter->drawPolygon(arrowHead);
    }
}