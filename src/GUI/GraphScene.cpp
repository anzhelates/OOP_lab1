/**
 * @file GraphScene.cpp
 * @brief Implementation of the GraphScene class to manage visual elements.
 */
#include "GraphScene.h"

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent), m_nextNodePos(50, 50), m_nodesInRow(0) {
    setSceneRect(0, 0, 800, 600);
}

GraphScene::~GraphScene() { clearScene(); }

void GraphScene::clearScene() {
    for (auto& pair : m_edgeItems) {
        removeItem(pair.second);
        delete pair.second;
    }
    m_edgeItems.clear();

    for (auto& pair : m_vertexItems) {
        removeItem(pair.second);
        delete pair.second;
    }
    m_vertexItems.clear();

    clear();
    m_nextNodePos = QPointF(50, 50);
    m_nodesInRow = 0;
}

VertexItem* GraphScene::addVertexItem(int id, const QString& label) {
    if (m_vertexItems.find(id) != m_vertexItems.end()) return nullptr;

    VertexItem* item = new VertexItem(id, label);
    addItem(item);
    item->setPos(m_nextNodePos);
    m_vertexItems[id] = item;

    m_nodesInRow++;
    m_nextNodePos.setX(m_nextNodePos.x() + NODE_SPACING * 2);
    if (m_nodesInRow >= MAX_NODES_PER_ROW) {
        m_nodesInRow = 0;
        m_nextNodePos.setX(50);
        m_nextNodePos.setY(m_nextNodePos.y() + NODE_SPACING * 2);
    }
    return item;
}

void GraphScene::removeVertexItem(int id) {
    auto it = m_vertexItems.find(id);
    if (it != m_vertexItems.end()) {
        VertexItem* item = it->second;
        removeItem(item);
        delete item;
        m_vertexItems.erase(it);
    }
}

EdgeItem* GraphScene::addEdgeItem(int fromId, int toId, double weight, bool isDirected, bool isWeighted) {
    auto pair = std::make_pair(fromId, toId);
    if (m_edgeItems.find(pair) != m_edgeItems.end()) return nullptr;

    VertexItem* source = getVertexItem(fromId);
    VertexItem* dest = getVertexItem(toId);
    if (!source || !dest) return nullptr;

    EdgeItem* edge = new EdgeItem(fromId, toId, source, dest, weight, isDirected, isWeighted);
    addItem(edge);
    m_edgeItems[pair] = edge;
    return edge;
}

void GraphScene::removeEdgeItem(int fromId, int toId) {
    auto pair = std::make_pair(fromId, toId);
    auto it = m_edgeItems.find(pair);
    if (it != m_edgeItems.end()) {
        EdgeItem* edge = it->second;
        removeItem(edge);
        delete edge;
        m_edgeItems.erase(it);
    }
}

VertexItem* GraphScene::getVertexItem(int id) const {
    auto it = m_vertexItems.find(id);
    return it != m_vertexItems.end() ? it->second : nullptr;
}

EdgeItem* GraphScene::getEdgeItem(int fromId, int toId) const {
    auto it = m_edgeItems.find(std::make_pair(fromId, toId));
    return it != m_edgeItems.end() ? it->second : nullptr;
}

void GraphScene::applyLayout(const QMap<int, QPointF>& positions) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        VertexItem* item = getVertexItem(it.key());
        if (item) item->setPos(it.value());
    }
}

void GraphScene::resetAlgorithmStyles() {
    for (auto& pair : m_vertexItems) {
        pair.second->resetColor();
        pair.second->hideDistanceText();
    }
    for (auto& pair : m_edgeItems) {
        pair.second->resetColor();
    }
}