#include "doctest.h"
#include "AdjacencyMatrix.h"
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

TEST_CASE("AdjacencyMatrix: addVertex and addEdge for directed graphs") {
    AdjacencyMatrix<Vertex, Edge> g(true);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");

    g.addVertex(a);
    g.addVertex(b);

    Edge* e = makeEdge(a, b, 8.0);
    g.addEdge(e);

    CHECK(g.getEdge(a->getId(), b->getId()) == e);
    CHECK(g.getEdge(b->getId(), a->getId()) == nullptr);

    CHECK(a->isActive());
    CHECK(b->isActive());
    CHECK(e->isActive());
}

TEST_CASE("AdjacencyMatrix: addEdge and getNeighbors for undirected graphs") {
    AdjacencyMatrix<Vertex, Edge> g(false);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");

    g.addVertex(a);
    g.addVertex(b);

    Edge* e = makeEdge(a, b, 12.0);
    g.addEdge(e);

    CHECK(g.getEdge(a->getId(), b->getId()) == e);
    CHECK(g.getEdge(b->getId(), a->getId()) == e);

    auto nA = g.getNeighbors(a->getId());
    auto nB = g.getNeighbors(b->getId());
    CHECK(nA.size() == 1);
    CHECK(nB.size() == 1);
    CHECK(nA[0] == b->getId());
    CHECK(nB[0] == a->getId());
}

TEST_CASE("AdjacencyMatrix: removeEdge and removeVertex") {
    AdjacencyMatrix<Vertex, Edge> g(false);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    Vertex* c = makeVertex("C");

    g.addVertex(a);
    g.addVertex(b);
    g.addVertex(c);

    Edge* e1 = makeEdge(a, b);
    Edge* e2 = makeEdge(b, c);
    g.addEdge(e1);
    g.addEdge(e2);

    g.removeEdge(e1);
    CHECK_FALSE(e1->isActive());
    CHECK(g.getEdge(a->getId(), b->getId()) == nullptr);

    g.removeVertex(b->getId());
    CHECK_FALSE(b->isActive());
    CHECK_FALSE(e2->isActive());
    CHECK(g.getEdge(a->getId(), b->getId()) == nullptr);
    CHECK(g.getEdge(b->getId(), c->getId()) == nullptr);
}

TEST_CASE("AdjacencyMatrix: getNeighbors, weights sum and invalid IDs") {
    AdjacencyMatrix<Vertex, Edge> g(true, true);
    Vertex* a = makeVertex("A");
    Vertex* b = makeVertex("B");
    Vertex* c = makeVertex("C");

    g.addVertex(a);
    g.addVertex(b);
    g.addVertex(c);

    Edge* e1 = makeEdge(a, b, 10.0);
    Edge* e2 = makeEdge(a, c, 20.0);
    g.addEdge(e1);
    g.addEdge(e2);

    auto nA = g.getNeighbors(a->getId());
    CHECK(nA.size() == 2);
    CHECK(std::find(nA.begin(), nA.end(), b->getId()) != nA.end());
    CHECK(std::find(nA.begin(), nA.end(), c->getId()) != nA.end());

    double sum = 0;
    for (Edge* edge : g.getEdges()) {
        if (edge->isActive()) {
            sum += edge->getWeight();
        }
    }
    CHECK(sum == 30.0);

    CHECK(g.getNeighbors(-1).empty());
    CHECK(g.getNeighbors(100).empty());
}