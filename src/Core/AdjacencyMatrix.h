#pragma once
#include "Graph.h"
#include <vector>
#include <stack>
#include <optional>
#include <algorithm>

namespace Core {

template <typename TVertex>
class AdjacencyMatrix : public Graph<TVertex> {
private:
    bool m_directed;
    bool m_weighted;
    std::vector<std::unique_ptr<TVertex>> m_vertices;
    std::vector<std::vector<std::optional<double>>> m_matrix;
    std::stack<int> m_freeIds;
    std::vector<GraphObserver*> m_observers;

    void notifyVertexAdded(int id) { for (auto obs : m_observers) obs->onVertexAdded(id); }
    void notifyVertexRemoved(int id) { for (auto obs : m_observers) obs->onVertexRemoved(id); }
    void notifyEdgeAdded(int from, int to, double weight) { for (auto obs : m_observers) obs->onEdgeAdded(from, to, weight); }
    void notifyEdgeRemoved(int from, int to) { for (auto obs : m_observers) obs->onEdgeRemoved(from, to); }
    void notifyGraphCleared() { for (auto obs : m_observers) obs->onGraphCleared(); }

public:
    explicit AdjacencyMatrix(bool directed = true, bool weighted = true)
        : m_directed(directed), m_weighted(weighted) {}

    bool isDirected() const override { return m_directed; }
    bool isWeighted() const override { return m_weighted; }

    int addVertex(std::unique_ptr<TVertex> vertex) override {
        if (!vertex) return -1;
        int id;
        if (!m_freeIds.empty()) {
            id = m_freeIds.top();
            m_freeIds.pop();
            vertex->setId(id);
            vertex->markActive();
            m_vertices[id] = std::move(vertex);

            for(int i = 0; i < m_matrix.size(); ++i) {
                m_matrix[id][i] = std::nullopt;
                m_matrix[i][id] = std::nullopt;
            }
        } else {
            id = static_cast<int>(m_vertices.size());
            vertex->setId(id);
            vertex->markActive();
            m_vertices.push_back(std::move(vertex));

            int newSize = m_vertices.size();
            m_matrix.resize(newSize);
            for (auto& row : m_matrix) {
                row.resize(newSize, std::nullopt);
            }
        }

        if (m_vertices[id]->getName().empty()) {
            m_vertices[id]->setName(std::to_string(id));
        }

        notifyVertexAdded(id);
        return id;
    }

    bool hasVertex(int id) const override {
        return id >= 0 && id < m_vertices.size() && m_vertices[id] && m_vertices[id]->isActive();
    }

    void removeVertex(int id) override {
        if (!hasVertex(id)) return;

        for (int i = 0; i < m_matrix.size(); ++i) {
            if (m_matrix[id][i].has_value()) removeEdge(id, i);
            if (m_matrix[i][id].has_value()) removeEdge(i, id);
        }

        m_vertices[id]->markInactive();
        m_freeIds.push(id);
        notifyVertexRemoved(id);
    }

    void addEdge(int from, int to, double weight = 1.0) override {
        if (!hasVertex(from) || !hasVertex(to)) return;
        if (m_matrix[from][to].has_value()) return;

        if (!m_weighted) weight = 1.0;

        m_matrix[from][to] = weight;
        notifyEdgeAdded(from, to, weight);

        if (!m_directed && from != to) {
            m_matrix[to][from] = weight;
            notifyEdgeAdded(to, from, weight);
        }
    }

    void removeEdge(int from, int to) override {
        if (from < 0 || from >= m_matrix.size() || to < 0 || to >= m_matrix.size()) return;

        if (m_matrix[from][to].has_value()) {
            m_matrix[from][to] = std::nullopt;
            notifyEdgeRemoved(from, to);
        }

        if (!m_directed && from != to && m_matrix[to][from].has_value()) {
            m_matrix[to][from] = std::nullopt;
            notifyEdgeRemoved(to, from);
        }
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
        for (int i = 0; i < m_matrix.size(); ++i) {
            if (!hasVertex(i)) continue;
            for (int j = (m_directed ? 0 : i); j < m_matrix.size(); ++j) {
                if (hasVertex(j) && m_matrix[i][j].has_value()) {
                    edges.emplace_back(i, j, m_matrix[i][j].value());
                }
            }
        }
        return edges;
    }

    std::vector<int> getNeighbors(int id) const override {
        std::vector<int> neighbors;
        if (hasVertex(id)) {
            for (int j = 0; j < m_matrix.size(); ++j) {
                if (hasVertex(j) && m_matrix[id][j].has_value()) {
                    neighbors.push_back(j);
                }
            }
        }
        return neighbors;
    }

    std::vector<Edge> getEdgesFrom(int id) const override {
        std::vector<Edge> edges;
        if (hasVertex(id)) {
            for (int j = 0; j < m_matrix.size(); ++j) {
                if (hasVertex(j) && m_matrix[id][j].has_value()) {
                    edges.emplace_back(id, j, m_matrix[id][j].value());
                }
            }
        }
        return edges;
    }

    bool hasEdge(int from, int to) const override {
        if (!hasVertex(from) || !hasVertex(to)) return false;
        return m_matrix[from][to].has_value();
    }

    double getEdgeWeight(int from, int to) const override {
        if (!hasVertex(from) || !hasVertex(to)) return 1.0;
        return m_matrix[from][to].value_or(1.0);
    }

    void clear() override {
        m_vertices.clear();
        m_matrix.clear();
        while (!m_freeIds.empty()) m_freeIds.pop();
        notifyGraphCleared();
    }

    int getVertexCount() const override {
        return m_vertices.size();
    }

    void addObserver(GraphObserver* observer) override {
        if (observer) m_observers.push_back(observer);
    }

    void removeObserver(GraphObserver* observer) override {
        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
    }
};
}