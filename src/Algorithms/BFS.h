#pragma once
#include "Algorithm.h"
#include <queue>
#include <vector>

namespace Algorithms {

    template<typename TVertex = Core::Vertex>
    class BFS : public Algorithm<TVertex> {
    public:
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

        bool isFinished() const override { return m_finished; }

    private:
        bool m_finished = false;
        std::vector<bool> m_visited;
        std::queue<int> m_queue;
    };
}