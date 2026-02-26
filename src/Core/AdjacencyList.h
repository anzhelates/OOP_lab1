#pragma once
#include "Graph.h"
#include <vector>
#include <list>
#include <stack>
#include <algorithm>
#include <string>

namespace Core {

template <typename TVertex = Vertex>
class AdjacencyList : public Graph<TVertex> {
private:
    bool m_directed;
    bool m_weighted;
    std::vector<std::unique_ptr<TVertex>> m_vertices;
    std::vector<std::list<Edge>> m_adjList;
    std::stack<int> m_freeIds;
    std::vector<GraphObserver*> m_observers;

    void notifyVertexAdded(int id) { for (auto obs : m_observers) obs->onVertexAdded(id); }
    void notifyVertexRemoved(int id) { for (auto obs : m_observers) obs->onVertexRemoved(id); }
    void notifyEdgeAdded(int from, int to, double weight) { for (auto obs : m_observers) obs->onEdgeAdded(from, to, weight); }
    void notifyEdgeRemoved(int from, int to) { for (auto obs : m_observers) obs->onEdgeRemoved(from, to); }

public:
    explicit AdjacencyList(bool directed = true, bool weighted = true)
        : m_directed(directed), m_weighted(weighted) {}

    bool isDirected() const override { return m_directed; }
    bool isWeighted() const override { return m_weighted; }

    bool hasVertex(int id) const override {
        return id >= 0 && id < m_vertices.size() && m_vertices[id] && m_vertices[id]->isActive();
    }

    int addVertex(std::unique_ptr<TVertex> vertex) override {
        if (!vertex) return -1;
        int id;
        if (!m_freeIds.empty()) {
            id = m_freeIds.top();
            m_freeIds.pop();
            vertex->setId(id);
            vertex->markActive();
            m_vertices[id] = std::move(vertex);
            m_adjList[id].clear();
        } else {
            id = static_cast<int>(m_vertices.size());
            vertex->setId(id);
            vertex->markActive();
            m_vertices.push_back(std::move(vertex));
            m_adjList.emplace_back();
        }

        if (m_vertices[id]->getName().empty()) {
            m_vertices[id]->setName(std::to_string(id));
        }

        notifyVertexAdded(id);
        return id;
    }

    void removeVertex(int id) override {
        if (!hasVertex(id)) return;
        m_vertices[id]->markInactive();

        std::vector<int> to_remove_out;
        for (const auto& edge : m_adjList[id]) {
            if (edge.isActive()) to_remove_out.push_back(edge.m_destination);
        }
        for (int to_id : to_remove_out) {
            removeEdge(id, to_id);
        }

        for (int i = 0; i < m_adjList.size(); ++i) {
            if (i == id || !hasVertex(i)) continue;
            std::vector<int> to_remove_in;
            for (const auto& edge : m_adjList[i]) {
                if (edge.m_destination == id && edge.isActive()) {
                    to_remove_in.push_back(i);
                }
            }
            for (int from_id : to_remove_in) {
                removeEdge(from_id, id);
            }
        }

        m_freeIds.push(id);
        notifyVertexRemoved(id);
    }

    void addEdge(int from, int to, double weight = 1.0) override {
        if (!hasVertex(from) || !hasVertex(to)) return;
        if (hasEdge(from, to)) return;
        if (!m_weighted) weight = 1.0;

        m_adjList[from].emplace_back(from, to, weight, true);
        notifyEdgeAdded(from, to, weight);
        if (!m_directed && from != to) {
            m_adjList[to].emplace_back(to, from, weight, true);
            notifyEdgeAdded(to, from, weight);
        }
    }

    void removeEdge(int from, int to) override {
        if (from < 0 || from >= m_adjList.size()) return;

        bool removed = false;
        for (auto& edge : m_adjList[from]) {
            if (edge.m_destination == to && edge.isActive()) {
                edge.markInactive();
                removed = true;
                break;
            }
        }
        if (removed) {
            notifyEdgeRemoved(from, to);
        }

        if (!m_directed && from != to && to < m_adjList.size()) {
            bool removedRev = false;
            for (auto& edge : m_adjList[to]) {
                if (edge.m_destination == from && edge.isActive()) {
                    edge.markInactive();
                    removedRev = true;
                    break;
                }
            }
            if (removedRev) {
                notifyEdgeRemoved(to, from);
            }
        }
    }

    bool hasEdge(int from, int to) const override {
        if (!hasVertex(from) || !hasVertex(to)) return false;
        for (const auto& edge : m_adjList[from]) {
            if (edge.m_destination == to && edge.isActive()) return true;
        }
        return false;
    }

    const TVertex* getVertex(int id) const override {
        if (hasVertex(id)) return m_vertices[id].get();
        return nullptr;
    }

    TVertex* getVertex(int id) override {
        if (hasVertex(id)) return m_vertices[id].get();
        return nullptr;
    }

    std::vector<const TVertex*> getVertices() const override {
        std::vector<const TVertex*> active;
        for (const auto& v : m_vertices) {
            if (v && v->isActive()) active.push_back(v.get());
        }
        return active;
    }

    std::vector<Edge> getEdges() const override {
        std::vector<Edge> edges;
        for (int i = 0; i < m_adjList.size(); ++i) {
            if (!hasVertex(i)) continue;
            for (const auto& edge : m_adjList[i]) {
                if (edge.isActive() && hasVertex(edge.m_destination)) {
                    if (!m_directed && edge.m_source > edge.m_destination) continue;
                    edges.push_back(edge);
                }
            }
        }
        return edges;
    }

    std::vector<int> getNeighbors(int id) const override {
        std::vector<int> neighbors;
        if (hasVertex(id)) {
            for (const auto& edge : m_adjList[id]) {
                if (edge.isActive() && hasVertex(edge.m_destination)) neighbors.push_back(edge.m_destination);
            }
        }
        return neighbors;
    }

    std::vector<Edge> getEdgesFrom(int id) const override {
        std::vector<Edge> edges;
        if (hasVertex(id)) {
            for (const auto& edge : m_adjList[id]) {
                if (edge.isActive() && hasVertex(edge.m_destination)) edges.push_back(edge);
            }
        }
        return edges;
    }

    double getEdgeWeight(int from, int to) const override {
        if (!hasVertex(from) || !hasVertex(to)) return std::numeric_limits<double>::infinity();
        for (const auto& edge : m_adjList[from]) {
            if (edge.m_destination == to && edge.isActive()) return edge.m_weight;
        }
        return 1.0;
    }

    void clear() override {
        m_vertices.clear();
        m_adjList.clear();
        while (!m_freeIds.empty()) m_freeIds.pop();
        for (auto obs : m_observers) obs->onGraphCleared();
    }

    int getVertexCount() const override { return m_vertices.size(); }

    void addObserver(GraphObserver* observer) override { if (observer) m_observers.push_back(observer); }
    void removeObserver(GraphObserver* observer) override {
        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
    }
};
}