/**
* @file Dijkstra.h
 * @brief Dijkstra's algorithm: shortest path algorithm for weighted graphs with non-negative edge weights.
 */

#pragma once
#include "Algorithm.h"
#include <queue>
#include <vector>
#include <limits>
#include <string>

namespace Algorithms {

/**
 * @brief Implements Dijkstra's algorithm for finding the shortest path in a graph.
 * * @tparam TVertex The vertex type used in the graph. Defaults to Core::Vertex.
 */
template<typename TVertex = Core::Vertex>
class Dijkstra : public Algorithm<TVertex> {
private:
    /**
     * @brief Internal helper structure to store a node and its computed distance.
     */
    struct NodeDist {
        int id;        ///< The ID of the vertex.
        double dist;   ///< The accumulated distance from the start vertex.

        /**
         * @brief Greater-than comparison operator, required for the min-priority queue.
         * * @param other The other NodeDist to compare with.
         * @return true If this distance is greater than the other distance.
         * @return false Otherwise.
         */
        bool operator>(const NodeDist& other) const {
            return dist > other.dist;
        }
    };

public:
    /**
     * @brief Runs Dijkstra's algorithm on the given graph.
     * * @param graph Pointer to the graph to run the algorithm on.
     * @param startId The ID of the starting vertex.
     * @param endId The ID of the target vertex. If specified, the algorithm stops upon finding its shortest path.
     * @param vertexCb Callback invoked during vertex state changes.
     * @param edgeCb Callback invoked during edge state changes.
     * @return true If the algorithm completed without initialization errors.
     * @return false If the graph is null or startId does not exist.
     */
    bool run(const Core::Graph<TVertex>* graph, int startId, int endId,
             typename Algorithm<TVertex>::VertexCallback vertexCb,
             typename Algorithm<TVertex>::EdgeCallback edgeCb) override
    {
        if (!graph || !graph->hasVertex(startId)) return false;

        m_finished = false;
        size_t maxId = graph->getVertexCount();
        std::vector<double> distances(maxId, std::numeric_limits<double>::infinity());
        std::vector<int> previous(maxId, -1);

        std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<NodeDist>> pq;

        distances[startId] = 0.0;
        pq.push({startId, 0.0});

        if (vertexCb) {
            vertexCb(startId, "dist=0");
            vertexCb(startId, "frontier");
        }

        while (!pq.empty()) {
            NodeDist current = pq.top();
            pq.pop();

            int u = current.id;
            if (!graph->hasVertex(u)) continue;
            if (current.dist > distances[u]) continue;

            if (vertexCb) vertexCb(u, "visiting");

            if (u == endId) {
                if (vertexCb) vertexCb(u, "visited");
                break;
            }

            std::vector<Core::Edge> edges = graph->getEdgesFrom(u);
            for (const auto& edge : edges) {
                int v = edge.getDestination();
                if (!graph->hasVertex(v)) continue;

                double weight = edge.getWeight();
                double newDist = distances[u] + weight;

                if (newDist < distances[v]) {
                    distances[v] = newDist;
                    previous[v] = u;
                    pq.push({v, newDist});

                    if (edgeCb) edgeCb(u, v, "tree");
                    if (vertexCb) {
                        vertexCb(v, "dist=" + std::to_string(newDist));
                        vertexCb(v, "frontier");
                    }
                }
            }
            if (vertexCb) vertexCb(u, "visited");
        }

        if (endId != -1 && distances[endId] != std::numeric_limits<double>::infinity()) {
            int curr = endId;
            while (curr != startId && curr != -1) {
                int prev = previous[curr];
                if (prev != -1) {
                    if (edgeCb) edgeCb(prev, curr, "path");
                }
                curr = prev;
            }
        }

        m_finished = true;
        return true;
    }

    /**
     * @brief Checks if Dijkstra's algorithm has completed execution.
     * * @return true If the execution has finished.
     * @return false Otherwise.
     */
    bool isFinished() const override { return m_finished; }

private:
    bool m_finished = false; ///< Flag indicating whether the algorithm has completed.
};
}