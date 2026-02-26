/**
* @file DFS.h
 * @brief Depth-First Search algorithm
 */

#pragma once
#include "Algorithm.h"
#include <stack>
#include <vector>

namespace Algorithms {

/**
 * @brief Implements the Depth-First Search (DFS) algorithm.
 * * @tparam TVertex The vertex type used in the graph. Defaults to Core::Vertex.
 */
template<typename TVertex = Core::Vertex>
class DFS : public Algorithm<TVertex> {
public:
    /**
     * @brief Runs the DFS algorithm on the given graph.
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
        m_stack = std::stack<int>();

        m_stack.push(startId);

        while (!m_stack.empty()) {
            int current = m_stack.top();
            m_stack.pop();

            if (!graph->hasVertex(current)) continue;

            if (!m_visited[current]) {
                m_visited[current] = true;
                if (vertexCb) vertexCb(current, "visiting");
                if (endId != -1 && current == endId) {
                    if (vertexCb) vertexCb(current, "visited");
                    break;
                }

                std::vector<int> neighbors = graph->getNeighbors(current);
                for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                    int neighbor = *it;
                    if (graph->hasVertex(neighbor) && !m_visited[neighbor]) {
                        m_stack.push(neighbor);
                        if (edgeCb) edgeCb(current, neighbor, "tree");
                        if (vertexCb) vertexCb(neighbor, "frontier");
                    }
                }
                if (vertexCb) vertexCb(current, "visited");
            }
        }

        m_finished = true;
        return true;
    }

    /**
     * @brief Checks if the DFS algorithm has completed execution.
     * * @return true If the execution has finished.
     * @return false Otherwise.
     */
    bool isFinished() const override { return m_finished; }

private:
    bool m_finished = false;         ///< Flag indicating whether the algorithm has completed.
    std::vector<bool> m_visited;     ///< Tracks which vertices have been visited to prevent processing duplicates.
    std::stack<int> m_stack;         ///< Stack used to maintain the DFS frontier.
};
}