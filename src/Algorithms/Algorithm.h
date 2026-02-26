/**
* @file Algorithm.h
 * @brief Base class representing a graph algorithm.
 */

#pragma once
#include "Graph.h"
#include <functional>
#include <string>

namespace Algorithms {

    /**
     * @brief Abstract base class for graph traversal and pathfinding algorithms.
     * * @tparam TVertex The vertex type used in the graph. Defaults to Core::Vertex.
     */
    template<typename TVertex = Core::Vertex>
    class Algorithm {
    public:
        /**
         * @brief Callback function type invoked when a vertex's state changes.
         * * @param vertexId The ID of the vertex whose state changed.
         * @param state A string describing the new state (e.g., "visiting", "visited", "frontier").
         */
        using VertexCallback = std::function<void(int vertexId, const std::string& state)>;

        /**
         * @brief Callback function type invoked when an edge's state changes.
         * * @param fromId The ID of the source vertex.
         * @param toId The ID of the destination vertex.
         * @param state A string describing the edge state (e.g., "tree", "path").
         */
        using EdgeCallback = std::function<void(int fromId, int toId, const std::string& state)>;

        /**
         * @brief Virtual destructor for proper cleanup of derived classes.
         */
        virtual ~Algorithm() = default;

        /**
         * @brief Executes the algorithm on the provided graph.
         * * @param graph Pointer to the graph to run the algorithm on.
         * @param startId The ID of the starting vertex.
         * @param endId The ID of the target vertex. Defaults to -1, which typically means traverse the whole graph.
         * @param vertexCb Callback invoked during vertex state changes. Defaults to nullptr.
         * @param edgeCb Callback invoked during edge state changes. Defaults to nullptr.
         * @return true If the algorithm was successfully initiated/completed.
         * @return false If the inputs are invalid (e.g., null graph, invalid startId).
         */
        virtual bool run(const Core::Graph<TVertex>* graph, int startId, int endId = -1,
                         VertexCallback vertexCb = nullptr,
                         EdgeCallback edgeCb = nullptr) = 0;

        /**
         * @brief Checks if the algorithm has finished its execution.
         * * @return true If the algorithm has completed.
         * @return false If the algorithm is still running or has not started.
         */
        virtual bool isFinished() const = 0;
    };
}