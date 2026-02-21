#pragma once
#include <QGraphicsScene>
#include <QMap>
#include "VertexItem.h"
#include "Graph.h"

class Vertex;
class Edge;
class EdgeItem;

using GuiGraph = Graph<Vertex, Edge>;

class GraphScene : public QGraphicsScene {
    Q_OBJECT
    public:
    explicit GraphScene(QObject* parent = nullptr);
    ~GraphScene();

    void setGraph(GuiGraph* graph);
    void clearScene();
    VertexItem* addVertex(Vertex* vertex);
    EdgeItem* addEdge(Edge* edge);
    void removeVertex(Vertex* vertex);
    void removeEdge(Edge* edge);

    VertexItem* getVertexItem(Vertex* vertex) const;
    EdgeItem* getEdgeItem(Edge* edge) const;

    void resetAlgorithmStyles();
    void updateEdgeWeights();
    void applyLayout(const QMap<int, QPointF>& positions);

private:
    GuiGraph* m_graph;
    QMap<Vertex*, VertexItem*> m_vertexItems;
    QMap<Edge*, EdgeItem*> m_edgeItems;
    QPointF m_nextNodePos;
    int m_nodesInRow;
    static constexpr int MAX_NODES_PER_ROW = 5;
    static constexpr qreal NODE_SPACING = 50.0;
};