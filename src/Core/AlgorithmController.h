#pragma once

#include "Graph.h"
#include "AlgorithmStep.h"
#include "GraphAlgorithm.h"

#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <functional>
#include <utility>
#include <algorithm>
#include <set>

enum class AlgorithmType { BFS, DFS, Dijkstra };

template <typename TVertex, typename TEdge>
class AlgorithmController {
public:
    using GuiGraph = Graph<TVertex, TEdge>;
    using DijkstraPair = std::pair<double, int>;

    explicit AlgorithmController(GuiGraph* graph = nullptr)
        : m_graph(graph), m_algorithm(AlgorithmType::BFS) {
        reset();
    }

    void setGraph(GuiGraph* graph) {
        m_graph = graph;
        reset();
    }

    void setAlgorithm(AlgorithmType alg) {
        m_algorithm = alg;
        reset();
    }

    AlgorithmType getAlgorithm() const {
        return m_algorithm;
    }

    size_t getCurrentStep() const {
        return m_currentStep;
    }

    bool start(int startVertexId, int endVertexId, AlgoState& state) {
        reset();
        if (!m_graph) return false;

        m_maxVertexId = m_graph->getVertexCount();
        if (m_maxVertexId == 0) return false;

        if (!m_graph->getVertexById(startVertexId)) {
            bool found = false;
            for(const auto& v : m_graph->getVertices()) {
                if(v && v->isActive()) {
                    startVertexId = v->getId();
                    found = true;
                    break;
                }
            }
            if(!found) return false;
        }

        m_startVertexId = startVertexId;
        m_endVertexId = (m_algorithm == AlgorithmType::Dijkstra) ? endVertexId : -1;
        m_algorithmFinished = false;
        initStates(startVertexId);

        AlgoState initialState;
        initialState.currentVertex = -1;
        initialState.message = "Algorithm started.";

        if (m_algorithm == AlgorithmType::BFS) {
            m_bfs_q.push(startVertexId);
            m_bfs_visited[startVertexId] = true;
            initialState.frontier.push_back(startVertexId);
            initialState.message = "BFS started at vertex " + std::to_string(startVertexId);
        }
        else if (m_algorithm == AlgorithmType::DFS) {
            m_dfs_s.push(startVertexId);
            initialState.frontier.push_back(startVertexId);
            initialState.message = "DFS started at vertex " + std::to_string(startVertexId);
        }
        else if (m_algorithm == AlgorithmType::Dijkstra) {
            m_dijkstra_dist[startVertexId] = 0.0;
            m_dijkstra_pq.push({0.0, startVertexId});

            initialState.distances = m_dijkstra_dist;
            initialState.predecessors = m_dijkstra_prev;
            initialState.frontier.push_back(startVertexId);

            std::string msg = "Dijkstra started at vertex " + std::to_string(startVertexId);
            if (m_endVertexId != -1) {
                msg += ", finding path to " + std::to_string(m_endVertexId);
            }
            initialState.message = msg;
        }

        m_history.push_back(initialState);
        state = initialState;
        return true;
    }

    bool nextStep(AlgoState& state) {
        if (!m_graph) return false;
        if (m_algorithmFinished) {
            return false;
        }

        if (m_currentStep + 1 < m_history.size()) {
            m_currentStep++;
            state = m_history[m_currentStep];
            return true;
        }

        AlgoState newState;
        if (!m_history.empty()) {
            newState = m_history.back();
        }
        newState.currentVertex = -1;
        newState.currentEdges.clear();
        newState.frontier.clear();
        newState.shortestPathEdges.clear();
        newState.message.clear();

        if (m_algorithm == AlgorithmType::BFS) {
            if (m_bfs_q.empty()) {
                m_algorithmFinished = true;
                newState.message = "BFS Finished";
            } else {
                int current = m_bfs_q.front();
                m_bfs_q.pop();

                if (!m_graph->getVertexById(current)) {
                    return nextStep(state);
                }

                m_allVisitedVertices.push_back(current);
                newState.currentVertex = current;
                newState.message = "Visiting " + std::to_string(current);

                std::vector<int> neighbors = m_graph->getNeighbors(current);
                for (int neighbor : neighbors) {
                    if (neighbor >= 0 && neighbor < m_maxVertexId && !m_bfs_visited[neighbor]) {
                        m_bfs_visited[neighbor] = true;
                        m_bfs_q.push(neighbor);

                        TEdge* edge = m_graph->getEdge(current, neighbor);
                        if (!edge && !m_graph->isDirected()) {
                            edge = m_graph->getEdge(neighbor, current);
                        }
                        if (edge) {
                            newState.currentEdges.push_back({edge->getFrom(), edge->getTo()});
                            m_allVisitedEdges.push_back({edge->getFrom(), edge->getTo()});
                        }
                    }
                }
                std::queue<int> temp_q = m_bfs_q;
                while(!temp_q.empty()) {
                    newState.frontier.push_back(temp_q.front());
                    temp_q.pop();
                }
            }
        }
        else if (m_algorithm == AlgorithmType::DFS) {
             int current = -1;
            while(!m_dfs_s.empty()) {
                current = m_dfs_s.top();
                m_dfs_s.pop();
                if (!m_graph->getVertexById(current)) continue;
                if (!m_dfs_visited[current]) break;
                current = -1;
            }

            if (current == -1) {
                m_algorithmFinished = true;
                newState.message = "DFS Finished";
            } else {
                m_dfs_visited[current] = true;
                m_allVisitedVertices.push_back(current);
                newState.currentVertex = current;
                newState.message = "Visiting " + std::to_string(current);

                std::vector<int> neighbors = m_graph->getNeighbors(current);
                for (int i = static_cast<int>(neighbors.size()) - 1; i >= 0; --i) {
                    int neighbor = neighbors[i];
                    if (neighbor >= 0 && neighbor < m_maxVertexId && !m_dfs_visited[neighbor]) {
                        m_dfs_s.push(neighbor);
                        TEdge* edge = m_graph->getEdge(current, neighbor);
                        if (!edge && !m_graph->isDirected()) {
                            edge = m_graph->getEdge(neighbor, current);
                        }
                        if (edge) {
                            newState.currentEdges.push_back({edge->getFrom(), edge->getTo()});
                            m_allVisitedEdges.push_back({edge->getFrom(), edge->getTo()});
                        }
                    }
                }
                std::stack<int> temp_s = m_dfs_s;
                while(!temp_s.empty()) {
                    newState.frontier.push_back(temp_s.top());
                    temp_s.pop();
                }
                std::reverse(newState.frontier.begin(), newState.frontier.end());
            }
        }
        else if (m_algorithm == AlgorithmType::Dijkstra) {
            int current = -1;
            while(!m_dijkstra_pq.empty()) {
                int pq_node = m_dijkstra_pq.top().second;
                double pq_dist = m_dijkstra_pq.top().first;
                m_dijkstra_pq.pop();

                if (!m_graph->getVertexById(pq_node)) continue;
                if (pq_dist > m_dijkstra_dist[pq_node]) continue;
                if (!m_dijkstra_visited[pq_node]) {
                    current = pq_node;
                    break;
                }
            }

            if (current == -1) {
                m_algorithmFinished = true;
                newState.message = "Dijkstra Finished";
                if (m_endVertexId != -1) {
                    newState.shortestPathEdges = reconstructPath(m_startVertexId, m_endVertexId);
                     if(newState.shortestPathEdges.empty() && m_startVertexId != m_endVertexId && m_dijkstra_dist[m_endVertexId] == std::numeric_limits<double>::infinity()) {
                        newState.message = "Dijkstra Finished. Path to " + std::to_string(m_endVertexId) + " not found.";
                    }
                }
            } else {
                m_dijkstra_visited[current] = true;
                m_allVisitedVertices.push_back(current);
                newState.currentVertex = current;
                newState.message = "Visiting " + std::to_string(current) + " (dist=" + std::to_string(m_dijkstra_dist[current]) + ")";

                if (current == m_endVertexId) {
                    m_algorithmFinished = true;
                    newState.message = "Dijkstra Finished. Path to " + std::to_string(m_endVertexId) + " found";
                    newState.shortestPathEdges = reconstructPath(m_startVertexId, m_endVertexId);
                } else {
                    std::vector<TEdge*> outgoingEdges = m_graph->getEdgesFrom(current);
                    for (TEdge* edge : outgoingEdges) {
                        int neighbor = edge->getDestination()->getId();
                        if (neighbor < 0 || neighbor >= m_maxVertexId || !m_graph->getVertexById(neighbor)) continue;
                        if (m_dijkstra_visited[neighbor]) continue;

                        double weight = m_graph->isWeighted() ? edge->getWeight() : 1.0;

                        double newDist = m_dijkstra_dist[current] + weight;

                        if (newDist < m_dijkstra_dist[neighbor]) {
                            m_dijkstra_dist[neighbor] = newDist;
                            m_dijkstra_prev[neighbor] = current;
                            m_dijkstra_pq.push({newDist, neighbor});
                            newState.currentEdges.push_back({edge->getFrom(), edge->getTo()});
                        }
                    }
                }

                m_allVisitedEdges.clear();
                for (int v = 0; v < m_maxVertexId; ++v) {
                    int u = m_dijkstra_prev[v];
                    if (u != -1 && m_graph->getVertexById(u) && m_graph->getVertexById(v)) {
                        m_allVisitedEdges.push_back({u, v});
                    }
                }

                auto temp_pq = m_dijkstra_pq;
                std::set<int> frontier_set;
                while(!temp_pq.empty()) {
                    int id = temp_pq.top().second;
                    temp_pq.pop();
                    if (m_graph->getVertexById(id) && !m_dijkstra_visited[id]) {
                        frontier_set.insert(id);
                    }
                }
                newState.frontier.assign(frontier_set.begin(), frontier_set.end());
            }
        }

        newState.visitedVertices = m_allVisitedVertices;
        newState.visitedEdges = m_allVisitedEdges;
        if (m_algorithm == AlgorithmType::Dijkstra) {
            newState.distances = m_dijkstra_dist;
            newState.predecessors = m_dijkstra_prev;
        }

        m_history.push_back(newState);
        m_currentStep++;
        state = newState;
        return true;
    }

    bool prevStep(AlgoState& state) {
        if (m_currentStep == 0) return false;
        m_algorithmFinished = false;

        m_currentStep--;
        state = m_history[m_currentStep];
        return true;
    }

    void reset() {
        m_currentStep = 0;
        m_history.clear();
        m_maxVertexId = 0;
        if(m_graph) {
            m_maxVertexId = m_graph->getVertexCount();
        }

        m_startVertexId = -1;
        m_endVertexId = -1;
        m_algorithmFinished = false;

        m_bfs_q = std::queue<int>();
        m_bfs_visited.assign(m_maxVertexId, false);

        m_dfs_s = std::stack<int>();
        m_dfs_visited.assign(m_maxVertexId, false);

        m_dijkstra_pq = std::priority_queue<DijkstraPair, std::vector<DijkstraPair>, std::greater<DijkstraPair>>();
        m_dijkstra_dist.assign(m_maxVertexId, std::numeric_limits<double>::infinity());
        m_dijkstra_prev.assign(m_maxVertexId, -1);
        m_dijkstra_visited.assign(m_maxVertexId, false);

        m_allVisitedVertices.clear();
        m_allVisitedEdges.clear();
    }

private:
    void initStates(int startVertexId) {
        m_bfs_q = std::queue<int>();
        m_bfs_visited.assign(m_maxVertexId, false);

        m_dfs_s = std::stack<int>();
        m_dfs_visited.assign(m_maxVertexId, false);

        m_dijkstra_pq = std::priority_queue<DijkstraPair, std::vector<DijkstraPair>, std::greater<DijkstraPair>>();
        m_dijkstra_dist.assign(m_maxVertexId, std::numeric_limits<double>::infinity());
        m_dijkstra_prev.assign(m_maxVertexId, -1);
        m_dijkstra_visited.assign(m_maxVertexId, false);

        m_allVisitedVertices.clear();
        m_allVisitedEdges.clear();
    }

    std::vector<EdgeId> reconstructPath(int startId, int endId) const {
        std::vector<EdgeId> path;
        int current = endId;

        if (endId < 0 || endId >= m_maxVertexId || m_dijkstra_prev.empty() || m_dijkstra_prev[current] == -1) {
            if (startId != endId) {
                return path;
            }
        }

        while (current != startId) {
            int prev = m_dijkstra_prev[current];
            if (prev == -1) {
                path.clear();
                break;
            }
            path.push_back({prev, current});
            current = prev;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    GuiGraph* m_graph = nullptr;
    AlgorithmType m_algorithm;

    std::vector<AlgoState> m_history;
    size_t m_currentStep = 0;
    int m_maxVertexId = 0;

    int m_startVertexId = -1;
    int m_endVertexId = -1;
    bool m_algorithmFinished = false;

    std::queue<int> m_bfs_q;
    std::vector<bool> m_bfs_visited;

    std::stack<int> m_dfs_s;
    std::vector<bool> m_dfs_visited;

    std::priority_queue<DijkstraPair, std::vector<DijkstraPair>, std::greater<DijkstraPair>> m_dijkstra_pq;
    std::vector<double> m_dijkstra_dist;
    std::vector<int> m_dijkstra_prev;
    std::vector<bool> m_dijkstra_visited;

    std::vector<int> m_allVisitedVertices;
    std::vector<EdgeId> m_allVisitedEdges;
};