/**
* @file Edge.h
 * @brief Represents a connection between two vertices in a graph.
 */

#pragma once
#include <functional>

namespace Core {

    /**
     * @brief Represents a directed or undirected connection between two vertices.
     */
    struct Edge {
        int m_source = -1;       ///< The ID of the source vertex.
        int m_destination = -1;  ///< The ID of the destination vertex.
        double m_weight = 1.0;   ///< The weight or cost of the edge.
        bool m_active = true;    ///< Flag indicating if the edge is active.

        /**
         * @brief Default constructor.
         */
        Edge() = default;

        /**
         * @brief Constructs an edge with specified properties.
         * @param source The source vertex ID.
         * @param destination The destination vertex ID.
         * @param weight The weight of the edge (default is 1.0).
         * @param active The active status of the edge (default is true).
         */
        Edge(int source, int destination, double weight = 1.0, bool active = true)
            : m_source(source), m_destination(destination), m_weight(weight), m_active(active) {}

        /**
         * @brief Gets the source vertex ID.
         * @return The source ID.
         */
        int getSource() const { return m_source; }

        /**
         * @brief Gets the destination vertex ID.
         * @return The destination ID.
         */
        int getDestination() const { return m_destination; }

        /**
         * @brief Gets the weight of the edge.
         * @return The edge weight.
         */
        double getWeight() const { return m_weight; }

        /**
         * @brief Marks the edge as inactive.
         */
        void markInactive() { m_active = false; }

        /**
         * @brief Marks the edge as active.
         */
        void markActive() { m_active = true; }

        /**
         * @brief Checks if the edge is active.
         * @return True if active, false otherwise.
         */
        bool isActive() const { return m_active; }

        /**
         * @brief Equality operator for edges.
         * @param other The other edge to compare with.
         * @return True if both source and destination match, false otherwise.
         */
        bool operator==(const Edge& other) const {
            return m_source == other.m_source && m_destination == other.m_destination;
        }
    };

    /**
     * @brief Hash function object for the Edge structure.
     */
    struct EdgeHash {
        /**
         * @brief Computes the hash value for an Edge.
         * @param e The edge to hash.
         * @return The computed hash value.
         */
        std::size_t operator()(const Edge& e) const {
            return std::hash<int>()(e.m_source) ^ (std::hash<int>()(e.m_destination) << 1);
        }
    };
}