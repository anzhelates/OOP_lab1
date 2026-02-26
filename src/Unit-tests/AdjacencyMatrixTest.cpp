/**
 * @file AdjacencyMatrixTest.cpp
 * @brief Unit tests for the AdjacencyMatrix class.
 */

#include "doctest.h"
#include "AdjacencyMatrix.h"
#include "Vertex.h"
#include "Edge.h"
#include <algorithm>
#include <memory>

using namespace Core;

/**
 * @brief Helper function to create a dynamically allocated Vertex.
 * @param name The string representation of the vertex name.
 * @return std::unique_ptr<Vertex> containing the created vertex.
 */
static std::unique_ptr<Vertex> makeVertex(const std::string& name) {
    return std::make_unique<Vertex>(name);
}

/**
 * @brief Tests basic construction and connectivity in a directed matrix context.
 */
TEST_CASE("AdjacencyMatrix: addVertex and addEdge for directed graphs") {
    AdjacencyMatrix<Vertex> g(true);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));

    g.addEdge(aId, bId, 8.0);

    CHECK(g.hasEdge(aId, bId));
    CHECK(g.getEdgeWeight(aId, bId) == 8.0);
    CHECK_FALSE(g.hasEdge(bId, aId));

    CHECK(g.getVertex(aId)->isActive());
    CHECK(g.getVertex(bId)->isActive());
}

/**
 * @brief Tests symmetric connectivity and neighbor querying in an undirected matrix.
 */
TEST_CASE("AdjacencyMatrix: addEdge and getNeighbors for undirected graphs") {
    AdjacencyMatrix<Vertex> g(false);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));

    g.addEdge(aId, bId, 12.0);

    CHECK(g.hasEdge(aId, bId));
    CHECK(g.hasEdge(bId, aId));
    CHECK(g.getEdgeWeight(aId, bId) == 12.0);

    auto nA = g.getNeighbors(aId);
    auto nB = g.getNeighbors(bId);
    CHECK(nA.size() == 1);
    CHECK(nB.size() == 1);
    CHECK(nA[0] == bId);
    CHECK(nB[0] == aId);
}

/**
 * @brief Validates removal logic for edges and vertices in an AdjacencyMatrix.
 */
TEST_CASE("AdjacencyMatrix: removeEdge and removeVertex") {
    AdjacencyMatrix<Vertex> g(false);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));
    int cId = g.addVertex(makeVertex("C"));

    g.addEdge(aId, bId);
    g.addEdge(bId, cId);

    g.removeEdge(aId, bId);
    CHECK_FALSE(g.hasEdge(aId, bId));

    Vertex* bPtr = g.getVertex(bId);
    g.removeVertex(bId);
    CHECK_FALSE(bPtr->isActive());
    CHECK_FALSE(g.hasEdge(bId, cId));
    CHECK_FALSE(g.hasEdge(aId, bId));
}

/**
 * @brief Confirms neighbor fetching counts correctly, sums weights, and ignores invalid IDs.
 */
TEST_CASE("AdjacencyMatrix: getNeighbors, weights sum and invalid IDs") {
    AdjacencyMatrix<Vertex> g(true, true);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));
    int cId = g.addVertex(makeVertex("C"));

    g.addEdge(aId, bId, 10.0);
    g.addEdge(aId, cId, 20.0);

    auto nA = g.getNeighbors(aId);
    CHECK(nA.size() == 2);
    CHECK(std::find(nA.begin(), nA.end(), bId) != nA.end());
    CHECK(std::find(nA.begin(), nA.end(), cId) != nA.end());

    double sum = 0;
    for (const auto& edge : g.getEdges()) {
        sum += edge.getWeight();
    }
    CHECK(sum == 30.0);

    CHECK(g.getNeighbors(-1).empty());
    CHECK(g.getNeighbors(100).empty());
}