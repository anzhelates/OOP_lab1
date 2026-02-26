#pragma once
#include "Graph.h"
#include <functional>
#include <string>

namespace Algorithms {

    template<typename TVertex = Core::Vertex>
    class Algorithm {
    public:
        using VertexCallback = std::function<void(int vertexId, const std::string& state)>;
        using EdgeCallback = std::function<void(int fromId, int toId, const std::string& state)>;

        virtual ~Algorithm() = default;

        virtual bool run(const Core::Graph<TVertex>* graph, int startId, int endId = -1,
                         VertexCallback vertexCb = nullptr,
                         EdgeCallback edgeCb = nullptr) = 0;

        virtual bool isFinished() const = 0;
    };
}