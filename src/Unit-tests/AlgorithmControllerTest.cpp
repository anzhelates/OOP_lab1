#include "doctest.h"
#include "AlgorithmController.h"
#include "AdjacencyList.h"
#include "Vertex.h"
#include "Edge.h"
#include <memory>

using namespace Core;
using namespace Algorithms;

TEST_SUITE("AlgorithmController: initialization") {
    TEST_CASE("Start with invalid vertex ID should return false") {
        AdjacencyList<Vertex> g(false);
        int idA = g.addVertex(std::make_unique<Vertex>("A"));

        AlgorithmController<Vertex> controller;
        controller.setGraph(&g);
        AlgoState state;
        bool started = controller.start(999, -1, state);

        CHECK(started == false);
    }

    TEST_CASE("Start on empty graph should fail") {
        AdjacencyList<Vertex> g;
        AlgorithmController<Vertex> controller;
        controller.setGraph(&g);
        AlgoState state;

        CHECK_FALSE(controller.start(0, -1, state));
    }
}

TEST_SUITE("AlgorithmController: BFS Stepping") {
    TEST_CASE("BFS step-by-Step State tracking") {
        AdjacencyList<Vertex> g(false);
        int a = g.addVertex(std::make_unique<Vertex>("A"));
        int b = g.addVertex(std::make_unique<Vertex>("B"));
        int c = g.addVertex(std::make_unique<Vertex>("C"));

        g.addEdge(a, b);
        g.addEdge(a, c);

        AlgorithmController<Vertex> controller;
        controller.setAlgorithm(AlgorithmType::BFS);
        controller.setGraph(&g);
        AlgoState state;

        controller.start(a, -1, state);
        CHECK(state.frontier.size() == 1);
        CHECK(state.frontier[0] == a);
        controller.nextStep(state);
        CHECK(state.currentVertex == a);

        for(int i = 0; i < 4; ++i) controller.nextStep(state);

        CHECK(state.frontier.size() == 2);

        controller.nextStep(state);
        CHECK(state.visitedVertices.size() == 1);
        CHECK(state.visitedVertices[0] == a);

        while(controller.nextStep(state));
        CHECK(controller.getCurrentStep() > 0);
    }
}

TEST_SUITE("AlgorithmController: Dijkstra's algorithm") {
    TEST_CASE("Dijkstra finds and reconstructs the correct path") {
        AdjacencyList<Vertex> g(false, true);
        int a = g.addVertex(std::make_unique<Vertex>("A"));
        int b = g.addVertex(std::make_unique<Vertex>("B"));
        int c = g.addVertex(std::make_unique<Vertex>("C"));

        g.addEdge(a, c, 10.0);
        g.addEdge(a, b, 2.0);
        g.addEdge(b, c, 2.0);

        AlgorithmController<Vertex> controller;
        controller.setAlgorithm(AlgorithmType::Dijkstra);
        controller.setGraph(&g);
        AlgoState state;

        controller.start(a, c, state);

        while(controller.nextStep(state));

        REQUIRE(state.shortestPathEdges.size() == 2);
        CHECK(state.shortestPathEdges[0].from == b);
        CHECK(state.shortestPathEdges[0].to == c);
        CHECK(state.shortestPathEdges[1].from == a);
        CHECK(state.shortestPathEdges[1].to == b);
    }
}

TEST_SUITE("AlgorithmController: history navigation") {
    TEST_CASE("Forward and backward consistency") {
        AdjacencyList<Vertex> g(false);
        int a = g.addVertex(std::make_unique<Vertex>("A"));
        int b = g.addVertex(std::make_unique<Vertex>("B"));
        g.addEdge(a, b);

        AlgorithmController<Vertex> controller;
        controller.setGraph(&g);
        AlgoState state;

        controller.start(a, -1, state);
        controller.nextStep(state);
        int vertexAtStep1 = state.currentVertex;
        int stepIdx = controller.getCurrentStep();
        controller.nextStep(state);
        bool success = controller.prevStep(state);

        CHECK(success == true);
        CHECK(controller.getCurrentStep() == stepIdx);
        CHECK(state.currentVertex == vertexAtStep1);
    }
}