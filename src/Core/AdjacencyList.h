#pragma once

#include "Graph.h"
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

template <typename TVertex, typename TEdge>
class AdjacencyList : public Graph<TVertex, TEdge> {
private:
    struct AdjEntry {
        int m_to;
        TEdge* m_edge;
        AdjEntry(int to, TEdge* edge) : m_to(to), m_edge(edge) {}
    };

    std::vector<std::list<AdjEntry>> m_adjList;
    std::stack<int> m_freeIds;

public:
    explicit AdjacencyList(bool directed = true, bool weighted = false)
        : Graph<TVertex, TEdge>(directed, weighted) {}

    ~AdjacencyList() override = default;

    int addVertex(TVertex* vertex) override {
        if (!vertex) return -1;

        int id;
        if (!m_freeIds.empty()) {
            id = m_freeIds.top();
            m_freeIds.pop();
            if(this->m_vertices[id]) delete this->m_vertices[id];
            this->m_vertices[id] = vertex;
            m_adjList[id].clear();
        } else {
            id = static_cast<int>(this->m_vertices.size());
            this->m_vertices.push_back(vertex);
            m_adjList.resize(this->m_vertices.size());
        }

        vertex->setId(id);
        vertex->markActive();
        return id;
    }

    void addEdge(TEdge* edge) override {
        if (!edge || !edge->getSource() || !edge->getDestination()) return;

        int from = edge->getSource()->getId();
        int to = edge->getDestination()->getId();

        if (from < 0 || from >= static_cast<int>(this->m_vertices.size()) ||
            to < 0 || to >= static_cast<int>(this->m_vertices.size())) {
            return;
        }

        if (!this->m_vertices[from]->isActive() || !this->m_vertices[to]->isActive()) {
            return;
        }

        edge->markActive();
        this->m_edges.push_back(edge);

        m_adjList[from].push_back(AdjEntry(to, edge));
        if (!this->m_directed) {
            m_adjList[to].push_back(AdjEntry(from, edge));
        }
    }

    void removeEdge(TEdge* edge) override {
        if (!edge || !edge->getSource() || !edge->getDestination()) return;

        int from = edge->getFrom();
        int to = edge->getTo();

        if (from >= 0 && from < static_cast<int>(m_adjList.size())) {
            m_adjList[from].remove_if([edge](const AdjEntry& entry) {
                return entry.m_edge == edge;
            });
        }
        if (!this->m_directed && to >= 0 && to < static_cast<int>(m_adjList.size())) {
             m_adjList[to].remove_if([edge](const AdjEntry& entry) {
                return entry.m_edge == edge;
            });
        }
        edge->markInactive();
    }

    void removeVertex(int id) override {
        if (id < 0 || id >= static_cast<int>(this->m_vertices.size()) ||
            !this->m_vertices[id] || !this->m_vertices[id]->isActive()) {
            return;
            }

        std::set<TEdge*> edgesToRemove;

        for (const auto& entry : m_adjList[id]) {
            if (entry.m_edge) {
                edgesToRemove.insert(entry.m_edge);
            }
        }
        m_adjList[id].clear();

        for (int i = 0; i < static_cast<int>(m_adjList.size()); ++i) {
            if (i == id) continue;

            for (auto it = m_adjList[i].begin(); it != m_adjList[i].end();) {
                if (it->m_to == id) {
                    if (it->m_edge) edgesToRemove.insert(it->m_edge);
                    it = m_adjList[i].erase(it);
                } else {
                    ++it;
                }
            }
        }

        for (TEdge* edge : edgesToRemove) {
            removeEdge(edge);
        }
        this->m_vertices[id]->markInactive();
        m_freeIds.push(id);
    }

    std::vector<int> getNeighbors(int id) const override {
        std::vector<int> neighbors;
        if (id >= 0 && id < static_cast<int>(m_adjList.size()) && this->m_vertices[id]->isActive()) {
            for (const auto& entry : m_adjList[id]) {
                if (entry.m_edge && entry.m_edge->isActive() &&
                    this->m_vertices[entry.m_to] && this->m_vertices[entry.m_to]->isActive()) {
                    neighbors.push_back(entry.m_to);
                }
            }
        }
        return neighbors;
    }

    TEdge* getEdge(int fromId, int toId) const override {
        if (fromId >= 0 && fromId < static_cast<int>(m_adjList.size())) {
            for (const auto& entry : m_adjList[fromId]) {
                if (entry.m_to == toId && entry.m_edge && entry.m_edge->isActive()) {
                     if (this->m_vertices[fromId] && this->m_vertices[fromId]->isActive() &&
                        this->m_vertices[toId] && this->m_vertices[toId]->isActive())
                    {
                         return entry.m_edge;
                    }
                }
            }
        }
        return nullptr;
    }

    std::vector<TEdge*> getEdgesFrom(int fromId) const override {
        std::vector<TEdge*> result;
        if (fromId >= 0 && fromId < static_cast<int>(m_adjList.size()) && this->m_vertices[fromId]->isActive()) {
            for (const auto& entry : this->m_adjList[fromId]) {
                if (entry.m_edge && entry.m_edge->isActive() &&
                    this->m_vertices[entry.m_to] && this->m_vertices[entry.m_to]->isActive())
                {
                    result.push_back(entry.m_edge);
                }
            }
        }
        return result;
    }
};