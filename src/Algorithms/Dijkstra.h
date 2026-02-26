#pragma once
#include "Algorithm.h"
#include <queue>
#include <vector>
#include <limits>
#include <string>

namespace Algorithms {

template<typename TVertex = Core::Vertex>
class Dijkstra : public Algorithm<TVertex> {
private:
    struct NodeDist {
        int id;
        double dist;
        bool operator>(const NodeDist& other) const {
            return dist > other.dist;
        }
    };

public:
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

    bool isFinished() const override { return m_finished; }

private:
    bool m_finished = false;
};
}