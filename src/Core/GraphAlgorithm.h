#pragma once

#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <functional>
#include "Graph.h"

class GraphAlgorithms {
public:
    template <typename TVertex, typename TEdge>
    static std::vector<int> BFS(const Graph<TVertex, TEdge>& graph, int startId) {
        std::vector<int> order;
        int n = graph.getVertexCount();
        if (startId < 0 || startId >= n || !graph.getVertexById(startId)) return order;

        std::vector<bool> visited(n, false);
        std::queue<int> q;

        visited[startId] = true;
        q.push(startId);

        while (!q.empty()) {
            int current = q.front(); q.pop();
            if(!graph.getVertexById(current)) continue;

            order.push_back(current);

            std::vector<int> neighbors = graph.getNeighbors(current);
            for (int neighbor : neighbors) {
                if (neighbor >= 0 && neighbor < n && !visited[neighbor] && graph.getVertexById(neighbor)) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        return order;
    }

    template <typename TVertex, typename TEdge>
    static std::vector<int> DFS(const Graph<TVertex, TEdge>& graph, int startId) {
        std::vector<int> order;
        int n = graph.getVertexCount();
        if (startId < 0 || startId >= n || !graph.getVertexById(startId)) return order;

        std::vector<bool> visited(n, false);
        std::stack<int> st;
        st.push(startId);

        while (!st.empty()) {
            int current = st.top(); st.pop();
            if(!graph.getVertexById(current)) continue;

            if (!visited[current]) {
                visited[current] = true;
                order.push_back(current);
                std::vector<int> neighbors = graph.getNeighbors(current);
                for (int i = static_cast<int>(neighbors.size()) - 1; i >= 0; --i) {
                    int neighbor = neighbors[i];
                     if (neighbor >= 0 && neighbor < n && !visited[neighbor] && graph.getVertexById(neighbor)) {
                        st.push(neighbor);
                    }
                }
            }
        }
        return order;
    }

    template <typename TVertex, typename TEdge>
    static std::vector<double> Dijkstra(const Graph<TVertex, TEdge>& graph, int startId,
                                        std::function<double(TEdge*)> getWeight)
    {
        int n = graph.getVertexCount();
        std::vector<double> dist(n, std::numeric_limits<double>::infinity());

        if (startId < 0 || startId >= n || !graph.getVertexById(startId)) {
            return dist;
        }

        dist[startId] = 0.0;

        using PDI = std::pair<double, int>;
        std::priority_queue<PDI, std::vector<PDI>, std::greater<PDI>> pq;

        pq.push({0.0, startId});

        while (!pq.empty()) {
            double currentDist = pq.top().first;
            int currentVertex = pq.top().second;
            pq.pop();

            if (!graph.getVertexById(currentVertex)) continue;

            if (currentDist > dist[currentVertex]) {
                continue;
            }

            std::vector<TEdge*> outgoingEdges = graph.getEdgesFrom(currentVertex);

            for (TEdge* edge : outgoingEdges) {
                int neighborVertex = edge->getDestination()->getId();
                if (!graph.getVertexById(neighborVertex)) continue;

                double edgeWeight = getWeight(edge);

                if (edgeWeight == std::numeric_limits<double>::infinity()) {
                    continue;
                }

                double newDist = dist[currentVertex] + edgeWeight;

                if (newDist < dist[neighborVertex]) {
                    dist[neighborVertex] = newDist;
                    pq.push({newDist, neighborVertex});
                }
            }
        }
        return dist;
    }

    template <typename TVertex, typename TEdge>
    static bool isConnected(const Graph<TVertex, TEdge>& graph) {
        int n = graph.getVertexCount();
        int activeCount = 0;
        int firstActive = -1;
        for(int i=0; i<n; ++i) {
            if(graph.getVertexById(i)) {
                activeCount++;
                if(firstActive == -1) firstActive = i;
            }
        }

        if (activeCount < 2) return true;

        std::vector<int> dfs_result = DFS(graph, firstActive);
        if (static_cast<int>(dfs_result.size()) != activeCount) {
             return false;
        }

        if (!graph.isDirected()) {
            return true;
        }

        for (int i = 0; i < n; ++i) {
            if(!graph.getVertexById(i)) continue;

            std::vector<int> result = DFS(graph, i);
            if (static_cast<int>(result.size()) != activeCount) {
                return false;
            }
        }

        return true;
    }
};