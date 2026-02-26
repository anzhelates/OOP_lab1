#include "doctest.h"
#include "AdjacencyList.h"
#include "Vertex.h"
#include "Edge.h"
#include <algorithm>
#include <memory>

using namespace Core;

static std::unique_ptr<Vertex> makeVertex(const std::string& name) {
    return std::make_unique<Vertex>(name);
}

TEST_CASE("AdjacencyList: basic vertex operations") {
    AdjacencyList<Vertex> g(false);

    CHECK(g.getVertexCount() == 0);

    int id1 = g.addVertex(makeVertex("A"));
    int id2 = g.addVertex(makeVertex("B"));

    CHECK(id1 == 0);
    CHECK(id2 == 1);
    CHECK(g.getVertex(id1)->getName() == "A");
    CHECK(g.getVertexCount() == 2);
    CHECK(g.getVertex(id1)->isActive());
    CHECK(g.getVertex(id2)->isActive());
}

TEST_CASE("AdjacencyList: undirected graph edge operations") {
    AdjacencyList<Vertex> g(false);

    int v1Id = g.addVertex(makeVertex("A"));
    int v2Id = g.addVertex(makeVertex("B"));
    int v3Id = g.addVertex(makeVertex("C"));
    g.addEdge(v1Id, v2Id, 5.0);
    g.addEdge(v2Id, v3Id, 7.0);

    SUBCASE("getNeighbors retrieves correct adjacent vertices") {
        auto n1 = g.getNeighbors(v1Id);
        auto n2 = g.getNeighbors(v2Id);
        auto n3 = g.getNeighbors(v3Id);

        CHECK(n1.size() == 1);
        CHECK(n1[0] == v2Id);

        CHECK(n2.size() == 2);
        CHECK(std::find(n2.begin(), n2.end(), v1Id) != n2.end());
        CHECK(std::find(n2.begin(), n2.end(), v3Id) != n2.end());

        CHECK(n3.size() == 1);
        CHECK(n3[0] == v2Id);
    }

    SUBCASE("hasEdge and getEdgeWeight work regardless of direction") {
        CHECK(g.hasEdge(v1Id, v2Id));
        CHECK(g.hasEdge(v2Id, v1Id));
        CHECK(g.getEdgeWeight(v1Id, v2Id) == 5.0);
        CHECK_FALSE(g.hasEdge(v1Id, v3Id));
    }

    SUBCASE("getEdgesFrom retrieves all incident edges") {
        auto edgesFrom2 = g.getEdgesFrom(v2Id);
        CHECK(edgesFrom2.size() == 2);

        bool foundE1 = false, foundE2 = false;
        for(const auto& e : edgesFrom2) {
            if (e.m_destination == v1Id && e.getWeight() == 5.0) foundE1 = true;
            if (e.m_destination == v3Id && e.getWeight() == 7.0) foundE2 = true;
        }
        CHECK(foundE1);
        CHECK(foundE2);
    }
}

TEST_CASE("AdjacencyList: directed graph edge operations") {
    AdjacencyList<Vertex> g(true);

    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));

    g.addEdge(aId, bId, 12.0);

    auto nA = g.getNeighbors(aId);
    auto nB = g.getNeighbors(bId);

    CHECK(nA.size() == 1);
    CHECK(nA[0] == bId);
    CHECK(nB.empty());

    CHECK(g.hasEdge(aId, bId));
    CHECK(g.getEdgeWeight(aId, bId) == 12.0);
    CHECK_FALSE(g.hasEdge(bId, aId));
}

TEST_CASE("AdjacencyList: removeEdge and removeVertex") {
    AdjacencyList<Vertex> g(false);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));
    int cId = g.addVertex(makeVertex("C"));

    g.addEdge(aId, bId, 5.0);
    g.addEdge(bId, cId, 7.0);
    g.addEdge(aId, cId, 15.0);

    SUBCASE("Removing an edge removes adjacency") {
        g.removeEdge(aId, bId);
        CHECK_FALSE(g.hasEdge(aId, bId));
        CHECK_FALSE(g.hasEdge(bId, aId));
    }

    SUBCASE("Removing a vertex deactivates incident edges and frees the ID") {
        Vertex* bPtr = g.getVertex(bId);
        g.removeVertex(bId);

        CHECK_FALSE(bPtr->isActive());
        CHECK_FALSE(g.hasEdge(bId, cId));
        CHECK(g.hasEdge(aId, cId));

        int dId = g.addVertex(makeVertex("D"));
        CHECK(dId == bId);
    }
}

TEST_CASE("AdjacencyList: invalid IDs and null checks") {
    AdjacencyList<Vertex> g(true);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));

    g.addEdge(aId, bId);

    CHECK(g.hasEdge(aId, bId));
    CHECK_FALSE(g.hasEdge(bId, aId));
    CHECK_FALSE(g.hasEdge(5, 1));
    CHECK_FALSE(g.hasEdge(-1, 0));
    CHECK(g.getVertex(99) == nullptr);
}

TEST_CASE("AdjacencyList: edge weights sum correctness") {
    AdjacencyList<Vertex> g(false, true);
    int aId = g.addVertex(makeVertex("A"));
    int bId = g.addVertex(makeVertex("B"));
    int cId = g.addVertex(makeVertex("C"));

    g.addEdge(aId, bId, 10.0);
    g.addEdge(bId, cId, 20.0);
    g.addEdge(aId, cId, 25.0);

    double totalWeight = 0;
    for (const auto& edge : g.getEdges()) {
        totalWeight += edge.getWeight();
    }
    CHECK(totalWeight == 55.0);
}