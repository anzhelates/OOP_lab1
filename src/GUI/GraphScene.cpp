#include "GraphScene.h"
#include "Graph.h"
#include "VertexItem.h"
#include "EdgeItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <utility>

using GuiGraph = Graph<Vertex, Edge>;

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent),
      m_graph(nullptr),
      m_nextNodePos(0, 0),
      m_nodesInRow(0)
{
    setSceneRect(-500, -500, 2000, 2000);
}

GraphScene::~GraphScene() {
    clearScene();
}

void GraphScene::setGraph(GuiGraph* graph) {
    clearScene();
    m_graph = graph;
}

void GraphScene::clearScene() {
    for (EdgeItem* item : std::as_const(m_edgeItems)) {
        if (item) {
            removeItem(item);
            delete item;
        }
    }
    m_edgeItems.clear();

    for (VertexItem* item : std::as_const(m_vertexItems)) {
        if (item) {
            removeItem(item);
            delete item;
        }
    }
    m_vertexItems.clear();
    m_graph = nullptr;
    m_nextNodePos = QPointF(0, 0);
    m_nodesInRow = 0;

    QGraphicsScene::clear();
}

VertexItem* GraphScene::addVertex(Vertex* vertex) {
    if (!m_graph || !vertex || m_vertexItems.contains(vertex)) {
        return nullptr;
    }

    VertexItem* item = new VertexItem(vertex);
    addItem(item);
    item->setPos(m_nextNodePos);

    m_nodesInRow++;
    m_nextNodePos.setX(m_nextNodePos.x() + NODE_SPACING + VertexItem::Radius * 2);
    if (m_nodesInRow >= MAX_NODES_PER_ROW) {
        m_nodesInRow = 0;
        m_nextNodePos.setX(0);
        m_nextNodePos.setY(m_nextNodePos.y() + NODE_SPACING + VertexItem::Radius * 2);
    }

    m_vertexItems.insert(vertex, item);
    return item;
}

EdgeItem* GraphScene::addEdge(Edge* edge) {
    if (!m_graph || !edge || m_edgeItems.contains(edge)) {
        return nullptr;
    }

    Vertex* v_src = edge->getSource();
    Vertex* v_dst = edge->getDestination();

    VertexItem* item_src = m_vertexItems.value(v_src, nullptr);
    VertexItem* item_dst = m_vertexItems.value(v_dst, nullptr);

    if (!item_src || !item_dst) {
        qWarning() << "GraphScene::addEdge: Cannot find VertexItems for edge";
        return nullptr;
    }

    EdgeItem* item = new EdgeItem(edge, item_src, item_dst,
                                  m_graph->isDirected(), m_graph->isWeighted());
    addItem(item);
    m_edgeItems.insert(edge, item);

    return item;
}

void GraphScene::removeVertex(Vertex* vertex) {
    if (!vertex) return;

    VertexItem* item = m_vertexItems.value(vertex, nullptr);
    if (!item) return;

    m_vertexItems.remove(vertex);
    removeItem(item);
    delete item;
}

void GraphScene::removeEdge(Edge* edge) {
    if (!edge) return;

    EdgeItem* item = m_edgeItems.value(edge, nullptr);
    if (!item) return;

    m_edgeItems.remove(edge);

    removeItem(item);
    delete item;
}

VertexItem* GraphScene::getVertexItem(Vertex* vertex) const {
    return m_vertexItems.value(vertex, nullptr);
}

EdgeItem* GraphScene::getEdgeItem(Edge* edge) const {
    return m_edgeItems.value(edge, nullptr);
}

void GraphScene::resetAlgorithmStyles() {
    for (VertexItem* item : std::as_const(m_vertexItems)) {
        item->resetColor();
        item->hideDistanceText();
        item->setZValue(1);
    }
    for (EdgeItem* item : std::as_const(m_edgeItems)) {
        item->resetColor();
        item->setZValue(0);
    }
}

void GraphScene::updateEdgeWeights() {
    for (EdgeItem* item : std::as_const(m_edgeItems)) {
        item->trackVertices();
    }
}

void GraphScene::applyLayout(const QMap<int, QPointF>& positions) {
    if (!m_graph) return;
    for (auto it = positions.constBegin(); it != positions.constEnd(); ++it) {
        int vertexId = it.key();
        const QPointF& pos = it.value();

        Vertex* v = m_graph->getVertexById(vertexId);
        if (v) {
            VertexItem* vItem = m_vertexItems.value(v, nullptr);
            if (vItem) {
                vItem->setPos(pos);
            }
        }
    }
    setSceneRect(itemsBoundingRect().marginsAdded(QMarginsF(50, 50, 50, 50)));
}