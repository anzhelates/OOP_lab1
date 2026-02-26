#pragma once
#include <vector>
#include <memory>
#include "Edge.h"
#include "Vertex.h"
#include "GraphObserver.h"

namespace Core {

    template <typename TVertex = Vertex>
    class Graph {
    public:
        virtual ~Graph() = default;

        virtual bool isDirected() const = 0;
        virtual bool isWeighted() const = 0;

        virtual int addVertex(std::unique_ptr<TVertex> vertex) = 0;
        virtual void removeVertex(int id) = 0;
        virtual void addEdge(int from, int to, double weight = 1.0) = 0;
        virtual void removeEdge(int from, int to) = 0;
        virtual void clear() = 0;

        virtual bool hasVertex(int id) const = 0;
        virtual bool hasEdge(int from, int to) const = 0;
        virtual const TVertex* getVertex(int id) const = 0;
        virtual TVertex* getVertex(int id) = 0;

        virtual std::vector<const TVertex*> getVertices() const = 0;
        virtual std::vector<Edge> getEdges() const = 0;
        virtual std::vector<int> getNeighbors(int id) const = 0;
        virtual std::vector<Edge> getEdgesFrom(int id) const = 0;
        virtual double getEdgeWeight(int from, int to) const = 0;
        virtual int getVertexCount() const = 0;

        virtual void addObserver(GraphObserver* observer) = 0;
        virtual void removeObserver(GraphObserver* observer) = 0;
    };
}