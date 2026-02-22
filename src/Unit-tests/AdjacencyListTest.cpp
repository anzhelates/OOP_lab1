#include "doctest.h"
#include "AdjacencyList.h"
#include "Vertex.h"
#include "Edge.h"
#include <algorithm>

static Edge* makeEdge(Vertex* from, Vertex* to, double weight = 1.0) {
    Edge* edge = new Edge(from, to, weight);
    edge->markActive();
    return edge;
}

static Vertex* makeVertex(const std::string& name) {
    return new Vertex(name);
}

TEST_CASE("AdjacencyList: basic vertex operations") {
    AdjacencyList<Vertex, Edge> g(false);

    CHECK(g.getVertexCount() == 0);

    Vertex* v1 = makeVertex("A");
    Vertex* v2 = makeVertex("B");

    int id1 = g.addVertex(v1);
    int id2 = g.addVertex(v2);

    CHECK(id1 == 0);
    CHECK(id2 == 1);
    CHECK(g.getVertexById(id1)->getName() == "A");
    CHECK(g.getVertexCount() == 2);
    CHECK(g.getVertexById(id1)->isActive());
    CHECK(g.getVertexById(id2)->isActive());
}

TEST_CASE("AdjacencyList: undirected graph edge operations") {
    AdjacencyList<Vertex, Edge> g(false);

    Vertex* v1 = makeVertex("A");
    Vertex* v2 = makeVertex("B");
    Vertex* v3 = makeVertex("C");

    g.addVertex(v1);
    g.addVertex(v2);
    g.addVertex(v3);

    Edge* e1 = makeEdge(v1, v2, 5.0);
    Edge* e2 = makeEdge(v2, v3, 7.0);

    g.addEdge(e1);
    g.addEdge(e2);

    SUBCASE("getNeighbors retrieves correct adjacent vertices") {
        auto n1 = g.getNeighbors(v1->getId());
        auto n2 = g.getNeighbors(v2->getId());
        auto n3 = g.getNeighbors(v3->getId());

        CHECK(n1.size() == 1);
        CHECK(n1[0] == v2->getId());

        CHECK(n2.size() == 2);
        CHECK(std::find(n2.begin(), n2.end(), v1->getId()) != n2.end());
        CHECK(std::find(n2.begin(), n2.end(), v3->getId()) != n2.end());

        CHECK(n3.size() == 1);
        CHECK(n3[0] == v2->getId());
    }

    SUBCASE("getEdge retrieves the correct edge regardless of direction") {
        CHECK(g.getEdge(v1->getId(), v2->getId()) == e1);
        CHECK(g.getEdge(v2->getId(), v1->getId()) == e1);
        CHECK(g.getEdge(v1->getId(), v3->getId()) == nullptr);
    }

    SUBCASE("getEdgesFrom retrieves all incident edges") {
        auto edgesFrom2 = g.getEdgesFrom(v2->getId());
        CHECK(edgesFrom2.size() == 2);
        CHECK((edgesFrom2[0] == e1 || edgesFrom2[1] == e1));
        CHECK((edgesFrom2[0] == e2 || edgesFrom2[1] == e2));
    }
}

TEST_CASE("AdjacencyList: directed graph edge operations") {
    AdjacencyList<Vertex, Edge> g(true);

    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    g.addVertex(a);
    g.addVertex(b);

    Edge* e = makeEdge(a, b, 12.0);
    g.addEdge(e);

    auto nA = g.getNeighbors(a->getId());
    auto nB = g.getNeighbors(b->getId());

    CHECK(nA.size() == 1);
    CHECK(nA[0] == b->getId());
    CHECK(nB.empty());

    CHECK(g.getEdge(a->getId(), b->getId())->getWeight() == 12.0);
    CHECK(g.getEdge(b->getId(), a->getId()) == nullptr);
}

TEST_CASE("AdjacencyList: removeEdge and removeVertex") {
    AdjacencyList<Vertex, Edge> g(false);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    Vertex* c = makeVertex("C");

    g.addVertex(a);
    g.addVertex(b);
    g.addVertex(c);

    Edge* e1 = makeEdge(a, b, 5.0);
    Edge* e2 = makeEdge(b, c, 7.0);
    Edge* e3 = makeEdge(a, c, 15.0);

    g.addEdge(e1);
    g.addEdge(e2);
    g.addEdge(e3);

    SUBCASE("Removing an edge deactivates it and removes adjacency") {
        g.removeEdge(e1);
        CHECK_FALSE(e1->isActive());
        CHECK(g.getEdge(a->getId(), b->getId()) == nullptr);
    }

    SUBCASE("Removing a vertex deactivates incident edges and frees the ID") {
        int bId = b->getId();
        g.removeVertex(bId);

        CHECK_FALSE(b->isActive());
        CHECK_FALSE(e2->isActive());
        CHECK(g.getEdge(a->getId(), c->getId()) == e3);

        Vertex* d = makeVertex("D");
        int dId = g.addVertex(d);
        CHECK(dId == bId);
    }
}

TEST_CASE("AdjacencyList: invalid IDs and null checks") {
    AdjacencyList<Vertex, Edge> g(true);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    g.addVertex(a);
    g.addVertex(b);

    Edge* e = makeEdge(a, b);
    g.addEdge(e);

    CHECK(g.getEdge(a->getId(), b->getId()) == e);
    CHECK(g.getEdge(b->getId(), a->getId()) == nullptr);
    CHECK(g.getEdge(5, 1) == nullptr);
    CHECK(g.getEdge(-1, 0) == nullptr);
    CHECK(g.getVertexById(99) == nullptr);
}

TEST_CASE("AdjacencyList: edge weights sum correctness") {
    AdjacencyList<Vertex, Edge> g(false, true);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    Vertex* c = makeVertex("C");

    g.addVertex(a);
    g.addVertex(b);
    g.addVertex(c);

    Edge* e1 = makeEdge(a, b, 10.0);
    Edge* e2 = makeEdge(b, c, 20.0);
    Edge* e3 = makeEdge(a, c, 25.0);

    g.addEdge(e1);
    g.addEdge(e2);
    g.addEdge(e3);

    double totalWeight = 0;
    for (Edge* edge : g.getEdges()) {
        if (edge->isActive()) {
            totalWeight += edge->getWeight();
        }
    }
    CHECK(totalWeight == 55.0);
}