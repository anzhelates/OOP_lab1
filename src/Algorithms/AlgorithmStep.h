/**
* @file AlgorithmStep.h
 * @brief Represents a single step of an algorithm execution.
 */

#pragma once
#include <vector>
#include <string>

/**
 * @brief Represents a directed edge between two vertices by their IDs.
 */
struct EdgeId {
    int from = -1; ///< The ID of the source vertex.
    int to = -1;   ///< The ID of the destination vertex.

    /**
     * @brief Less-than comparison operator for sorting and using EdgeId in standard containers like std::set or std::map.
     * * @param other The other EdgeId to compare with.
     * @return true If this edge is strictly less than the other edge.
     * @return false Otherwise.
     */
    bool operator<(const EdgeId& other) const {
        if (from != other.from) return from < other.from;
        return to < other.to;
    }

    /**
     * @brief Equality comparison operator.
     * * @param other The other EdgeId to compare with.
     * @return true If both source and destination IDs are equal.
     * @return false Otherwise.
     */
    bool operator==(const EdgeId& other) const {
        return from == other.from && to == other.to;
    }
};

/**
 * @brief Represents a snapshot of the algorithm's state at a specific step.
 * * This structure is used to track the progress of graph traversal algorithms
 * for visualization or step-by-step execution.
 */
struct AlgoState {
    int currentVertex = -1;                 ///< The ID of the vertex currently being processed.
    std::vector<int> frontier;              ///< The list of vertices discovered but not yet processed (e.g., in the queue or stack).
    std::vector<int> visitedVertices;       ///< The list of vertices that have been completely processed.
    std::vector<EdgeId> visitedEdges;       ///< The list of edges that form the traversal tree.
    std::vector<double> distances;          ///< The computed distances from the start vertex to each vertex (used in Dijkstra).
    std::vector<EdgeId> shortestPathEdges;  ///< The list of edges that belong to the final shortest path.
    std::string message;                    ///< Optional informational message regarding the current state.
};