#pragma once

namespace Core {

    class GraphObserver {
    public:
        virtual ~GraphObserver() = default;

        virtual void onVertexAdded(int id) = 0;
        virtual void onVertexRemoved(int id) = 0;
        virtual void onEdgeAdded(int from, int to, double weight) = 0;
        virtual void onEdgeRemoved(int from, int to) = 0;
        virtual void onGraphCleared() = 0;
    };
}