/**
* @file Graph.h
 * @brief Main data structure.
 */

#pragma once
#include <vector>
#include <memory>
#include "Edge.h"
#include "Vertex.h"
#include "GraphObserver.h"

namespace Core {

    /**
     * @brief Abstract base class defining the interface for a graph data structure.
     * @tparam TVertex The type of vertex used in the graph, defaults to Core::Vertex.
     */
    template <typename TVertex = Vertex>
    class Graph {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Graph() = default;

        /**
         * @brief Checks if the graph is directed.
         * @return True if directed, false otherwise.
         */
        virtual bool isDirected() const = 0;

        /**
         * @brief Checks if the graph has weighted edges.
         * @return True if weighted, false otherwise.
         */
        virtual bool isWeighted() const = 0;

        /**
         * @brief Adds a new vertex to the graph.
         * @param vertex A unique pointer to the vertex to add.
         * @return The ID assigned to the newly added vertex, or -1 on failure.
         */
        virtual int addVertex(std::unique_ptr<TVertex> vertex) = 0;

        /**
         * @brief Removes a vertex from the graph by ID.
         * @param id The ID of the vertex to remove.
         */
        virtual void removeVertex(int id) = 0;

        /**
         * @brief Adds an edge between two vertices.
         * @param from The source vertex ID.
         * @param to The destination vertex ID.
         * @param weight The weight of the edge (default is 1.0).
         */
        virtual void addEdge(int from, int to, double weight = 1.0) = 0;

        /**
         * @brief Removes an edge between two vertices.
         * @param from The source vertex ID.
         * @param to The destination vertex ID.
         */
        virtual void removeEdge(int from, int to) = 0;

        /**
         * @brief Clears the entire graph, removing all vertices and edges.
         */
        virtual void clear() = 0;

        /**
         * @brief Checks if a vertex exists in the graph.
         * @param id The ID of the vertex.
         * @return True if the vertex exists and is active, false otherwise.
         */
        virtual bool hasVertex(int id) const = 0;

        /**
         * @brief Checks if an edge exists between two vertices.
         * @param from The source vertex ID.
         * @param to The destination vertex ID.
         * @return True if the edge exists, false otherwise.
         */
        virtual bool hasEdge(int from, int to) const = 0;

        /**
         * @brief Gets a constant pointer to a vertex by ID.
         * @param id The ID of the vertex.
         * @return A constant pointer to the vertex, or nullptr if not found.
         */
        virtual const TVertex* getVertex(int id) const = 0;

        /**
         * @brief Gets a pointer to a vertex by ID.
         * @param id The ID of the vertex.
         * @return A pointer to the vertex, or nullptr if not found.
         */
        virtual TVertex* getVertex(int id) = 0;

        /**
         * @brief Gets a list of all active vertices in the graph.
         * @return A vector of constant pointers to the active vertices.
         */
        virtual std::vector<const TVertex*> getVertices() const = 0;

        /**
         * @brief Gets a list of all active edges in the graph.
         * @return A vector of active edges.
         */
        virtual std::vector<Edge> getEdges() const = 0;

        /**
         * @brief Gets a list of neighbor vertex IDs for a given vertex.
         * @param id The ID of the target vertex.
         * @return A vector of neighbor vertex IDs.
         */
        virtual std::vector<int> getNeighbors(int id) const = 0;

        /**
         * @brief Gets all outgoing edges from a specific vertex.
         * @param id The ID of the source vertex.
         * @return A vector of edges originating from the given vertex.
         */
        virtual std::vector<Edge> getEdgesFrom(int id) const = 0;

        /**
         * @brief Gets the weight of the edge between two vertices.
         * @param from The source vertex ID.
         * @param to The destination vertex ID.
         * @return The weight of the edge, or infinity if it doesn't exist.
         */
        virtual double getEdgeWeight(int from, int to) const = 0;

        /**
         * @brief Gets the total count of vertices (including inactive ones that haven't been reused).
         * @return The capacity/size of the vertex container.
         */
        virtual int getVertexCount() const = 0;

        /**
         * @brief Registers an observer to receive graph modification events.
         * @param observer Pointer to the observer instance.
         */
        virtual void addObserver(GraphObserver* observer) = 0;

        /**
         * @brief Unregisters a previously added observer.
         * @param observer Pointer to the observer instance to remove.
         */
        virtual void removeObserver(GraphObserver* observer) = 0;
    };
}