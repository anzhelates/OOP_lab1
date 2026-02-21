#pragma once

#include "Graph.h"
#include <vector>
#include <algorithm>
#include <set>

template <typename TVertex, typename TEdge>
class AdjacencyMatrix : public Graph<TVertex, TEdge> {
private:
    std::vector<std::vector<TEdge*>> m_adjMatrix;

public:
    explicit AdjacencyMatrix(bool directed = true, bool weighted = false)
        : Graph<TVertex, TEdge>(directed, weighted) {}

    int addVertex(TVertex* vertex) override {
        if (!vertex) return -1;

        int id = static_cast<int>(this->m_vertices.size());
        vertex->setId(id);
        vertex->markActive();
        this->m_vertices.push_back(vertex);

        m_adjMatrix.resize(this->m_vertices.size());
        for (auto& row : m_adjMatrix) {
            row.resize(this->m_vertices.size(), nullptr);
        }
        return id;
    }

    void addEdge(TEdge* edge) override {
        if (!edge || !edge->getSource() || !edge->getDestination()) return;
        int from = edge->getSource()->getId();
        int to = edge->getDestination()->getId();
        if (from < 0 || from >= static_cast<int>(this->m_vertices.size())) return;
        if (to < 0 || to >= static_cast<int>(this->m_vertices.size())) return;

        if (!this->m_vertices[from]->isActive() || !this->m_vertices[to]->isActive()) {
            return;
        }

        if (m_adjMatrix[from][to] != nullptr) return;

        edge->markActive();
        this->m_edges.push_back(edge);
        m_adjMatrix[from][to] = edge;
        if (!this->m_directed) {
            m_adjMatrix[to][from] = edge;
        }
    }

    void removeEdge(TEdge* edge) override {
        if (!edge) return;

        int from = edge->getFrom();
        int to = edge->getTo();

        if (from >= 0 && from < static_cast<int>(m_adjMatrix.size()) &&
            to >= 0 && to < static_cast<int>(m_adjMatrix.size())) {
            if (m_adjMatrix[from][to] == edge) m_adjMatrix[from][to] = nullptr;
            if (!this->m_directed && m_adjMatrix[to][from] == edge) m_adjMatrix[to][from] = nullptr;
        }
        edge->markInactive();
        this->m_edges.erase(std::remove(this->m_edges.begin(), this->m_edges.end(), edge), this->m_edges.end());
    }

    void removeVertex(int id) override {
         if (id < 0 || id >= static_cast<int>(this->m_vertices.size()) || !this->m_vertices[id]->isActive()) {
            return;
        }

        std::set<TEdge*> edgesToRemove;
        for (int i = 0; i < static_cast<int>(m_adjMatrix.size()); ++i) {
            if (m_adjMatrix[id][i] != nullptr) {
                edgesToRemove.insert(m_adjMatrix[id][i]);
                m_adjMatrix[id][i] = nullptr;
            }
            if (m_adjMatrix[i][id] != nullptr) {
                edgesToRemove.insert(m_adjMatrix[i][id]);
                m_adjMatrix[i][id] = nullptr;
            }
        }

        for (TEdge* edge : edgesToRemove) {
            removeEdge(edge);
        }

        if (this->m_vertices[id]) {
            this->m_vertices[id]->markInactive();
        }
    }

    std::vector<int> getNeighbors(int id) const override {
        std::vector<int> neighbors;
        if (id >= 0 && id < static_cast<int>(m_adjMatrix.size()) && this->m_vertices[id]->isActive()) {
            for (int i = 0; i < static_cast<int>(m_adjMatrix[id].size()); ++i) {
                if (m_adjMatrix[id][i] != nullptr && m_adjMatrix[id][i]->isActive() &&
                    this->m_vertices[i] && this->m_vertices[i]->isActive()) {
                    neighbors.push_back(i);
                }
            }
        }
        return neighbors;
    }

    TEdge* getEdge(int fromId, int toId) const override {
        if (fromId >= 0 && fromId < static_cast<int>(m_adjMatrix.size())
            && toId >= 0 && toId < static_cast<int>(m_adjMatrix.size())) {

            TEdge* e = m_adjMatrix[fromId][toId];
            if (e && e->isActive() &&
                this->m_vertices[fromId] && this->m_vertices[fromId]->isActive() &&
                this->m_vertices[toId] && this->m_vertices[toId]->isActive())
            {
                return e;
            }
        }
        return nullptr;
    }

    std::vector<TEdge*> getEdgesFrom(int fromId) const override {
        std::vector<TEdge*> result;
        if (fromId >= 0 && fromId < static_cast<int>(m_adjMatrix.size()) && this->m_vertices[fromId]->isActive()) {
            for (size_t to = 0; to < m_adjMatrix[fromId].size(); ++to) {
                TEdge* edge = m_adjMatrix[fromId][to];
                if (edge && edge->isActive() &&
                    this->m_vertices[to] && this->m_vertices[to]->isActive()) {
                    result.push_back(edge);
                }
            }
        }
        return result;
    }
};