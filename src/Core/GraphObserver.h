/**
* @file GraphObserver.h
 * @brief Observer interface for receiving graph update notifications.
 */

#pragma once

namespace Core {

    /**
     * @brief Interface for observing changes in a graph.
     * * Implement this interface to receive notifications when vertices
     * or edges are added, removed, or when the graph is cleared.
     */
    class GraphObserver {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~GraphObserver() = default;

        /**
         * @brief Called when a new vertex is added to the graph.
         * @param id The ID of the newly added vertex.
         */
        virtual void onVertexAdded(int id) = 0;

        /**
         * @brief Called when a vertex is removed from the graph.
         * @param id The ID of the removed vertex.
         */
        virtual void onVertexRemoved(int id) = 0;

        /**
         * @brief Called when a new edge is added to the graph.
         * @param from The ID of the source vertex.
         * @param to The ID of the destination vertex.
         * @param weight The weight of the added edge.
         */
        virtual void onEdgeAdded(int from, int to, double weight) = 0;

        /**
         * @brief Called when an edge is removed from the graph.
         * @param from The ID of the source vertex.
         * @param to The ID of the destination vertex.
         */
        virtual void onEdgeRemoved(int from, int to) = 0;

        /**
         * @brief Called when the entire graph is cleared.
         */
        virtual void onGraphCleared() = 0;
    };
}