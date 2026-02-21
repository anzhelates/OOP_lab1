#pragma once

#include <vector>
#include <string>

class VertexTransport;
class EdgeTransport;
class Vertex;
class Edge;

template <typename TVertex, typename TEdge>
class Graph {
protected:
    std::vector<TVertex*> m_vertices;
    std::vector<TEdge*> m_edges;
    bool m_directed;
    bool m_weighted;

public:
    explicit Graph(bool directed = true, bool weighted = false)
        : m_directed(directed), m_weighted(weighted) {}

    virtual ~Graph() {
        for (auto* edge : m_edges) { delete edge; }
        for (auto* vertex : m_vertices) { delete vertex; }
    }

    virtual int addVertex(TVertex* vertex) = 0;
    virtual void addEdge(TEdge* edge) = 0;
    virtual void removeVertex(int id) = 0;
    virtual void removeEdge(TEdge* edge) = 0;

    bool isDirected() const { return m_directed; }
    bool isWeighted() const { return m_weighted; }

    int getVertexCount() const { return static_cast<int>(m_vertices.size()); }

    TVertex* getVertexById(int id) const {
        if (id >= 0 && id < static_cast<int>(m_vertices.size())) {
            if (m_vertices[id] && m_vertices[id]->isActive()) {
                return m_vertices[id];
            }
        }
        return nullptr;
    }

    virtual std::vector<int> getNeighbors(int id) const = 0;
    virtual TEdge* getEdge(int fromId, int toId) const = 0;
    virtual std::vector<TEdge*> getEdgesFrom(int fromId) const = 0;

    const std::vector<TVertex*>& getVertices() const { return m_vertices; }
    const std::vector<TEdge*>& getEdges() const { return m_edges; }
};