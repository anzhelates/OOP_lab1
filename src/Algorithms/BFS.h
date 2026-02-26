/**
* @file BFS.h
 * @brief Breadth-First Search algorithm
 */

#pragma once
#include "Algorithm.h"
#include <queue>
#include <vector>

namespace Algorithms {

    /**
     * @brief Implements the Breadth-First Search (BFS) algorithm.
     * * @tparam TVertex The vertex type used in the graph. Defaults to Core::Vertex.
     */
    template<typename TVertex = Core::Vertex>
    class BFS : public Algorithm<TVertex> {
    public:
        /**
         * @brief Runs the BFS algorithm on the given graph.
         * * @param graph Pointer to the graph to traverse.
         * @param startId The ID of the starting vertex.
         * @param endId The ID of the target vertex. Traversal stops early if this is reached.
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
            m_visited.assign(maxId, false);
            m_queue = std::queue<int>();

            m_queue.push(startId);
            m_visited[startId] = true;

            if (vertexCb) vertexCb(startId, "frontier");

            while (!m_queue.empty()) {
                int current = m_queue.front();
                m_queue.pop();

                if (!graph->hasVertex(current)) continue;

                if (vertexCb) vertexCb(current, "visiting");

                std::vector<int> neighbors = graph->getNeighbors(current);
                for (int neighbor : neighbors) {
                    if (graph->hasVertex(neighbor) && !m_visited[neighbor]) {
                        m_visited[neighbor] = true;
                        m_queue.push(neighbor);

                        if (edgeCb) edgeCb(current, neighbor, "tree");
                        if (vertexCb) vertexCb(neighbor, "frontier");
                    }
                }
                if (vertexCb) vertexCb(current, "visited");
                if (endId != -1 && current == endId) break;
            }

            m_finished = true;
            return true;
        }

        /**
         * @brief Checks if the BFS algorithm has completed execution.
         * * @return true If the execution has finished.
         * @return false Otherwise.
         */
        bool isFinished() const override { return m_finished; }

    private:
        bool m_finished = false;           ///< Flag indicating whether the algorithm has completed.
        std::vector<bool> m_visited;       ///< Tracks which vertices have been visited to prevent processing duplicates.
        std::queue<int> m_queue;           ///< Queue used to maintain the BFS frontier.
    };
}