#pragma once
#include "Algorithm.h"
#include <stack>
#include <vector>

namespace Algorithms {

template<typename TVertex = Core::Vertex>
class DFS : public Algorithm<TVertex> {
public:
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

    bool isFinished() const override { return m_finished; }

private:
    bool m_finished = false;
    std::vector<bool> m_visited;
    std::stack<int> m_stack;
};
}