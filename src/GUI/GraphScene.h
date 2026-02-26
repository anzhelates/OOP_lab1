#pragma once
#include <QGraphicsScene>
#include <unordered_map>
#include "VertexItem.h"
#include "EdgeItem.h"

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);
    ~GraphScene() override;

    void clearScene();

    VertexItem* addVertexItem(int id, const QString& label);
    EdgeItem* addEdgeItem(int fromId, int toId, double weight, bool isDirected, bool isWeighted);

    void removeVertexItem(int id);
    void removeEdgeItem(int fromId, int toId);

    VertexItem* getVertexItem(int id) const;
    EdgeItem* getEdgeItem(int fromId, int toId) const;

    void applyLayout(const QMap<int, QPointF>& positions);
    void resetAlgorithmStyles();

private:
    std::unordered_map<int, VertexItem*> m_vertexItems;
    std::unordered_map<std::pair<int, int>, EdgeItem*, PairHash> m_edgeItems;

    QPointF m_nextNodePos;
    int m_nodesInRow;
    static constexpr int MAX_NODES_PER_ROW = 5;
    static constexpr qreal NODE_SPACING = 60.0;
};