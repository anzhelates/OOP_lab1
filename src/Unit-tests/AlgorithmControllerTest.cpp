#include "doctest.h"
#include "AlgorithmController.h"
#include "AdjacencyList.h"
#include "Vertex.h"
#include "Edge.h"

TEST_SUITE("AlgorithmController: initialization") {
    TEST_CASE("Start with invalid vertex ID, should fallback to first active") {
        AdjacencyList<Vertex, Edge> g(false);
        Vertex* vA = new Vertex("A");
        int idA = g.addVertex(vA);

        AlgorithmController<Vertex, Edge> controller(&g);
        AlgoState state;

        bool started = controller.start(999, -1, state);

        CHECK(started == true);
        CHECK(state.frontier.size() == 1);
        CHECK(state.frontier[0] == idA);
    }

    TEST_CASE("Start on empty graph should fail") {
        AdjacencyList<Vertex, Edge> g;
        AlgorithmController<Vertex, Edge> controller(&g);
        AlgoState state;

        CHECK_FALSE(controller.start(0, -1, state));
    }
}

TEST_SUITE("AlgorithmController: BFS Stepping") {
    TEST_CASE("BFS step-by-Step State tracking") {
        AdjacencyList<Vertex, Edge> g(false);
        int a = g.addVertex(new Vertex("A"));
        int b = g.addVertex(new Vertex("B"));
        int c = g.addVertex(new Vertex("C"));
        g.addEdge(new Edge(g.getVertexById(a), g.getVertexById(b)));
        g.addEdge(new Edge(g.getVertexById(a), g.getVertexById(c)));

        AlgorithmController<Vertex, Edge> controller(&g);
        controller.setAlgorithm(AlgorithmType::BFS);
        AlgoState state;

        controller.start(a, -1, state);
        CHECK(state.frontier.size() == 1);

        controller.nextStep(state);
        CHECK(state.currentVertex == a);
        CHECK(state.frontier.size() == 2);
        CHECK(state.visitedVertices.size() == 1);

        controller.nextStep(state);
        CHECK(state.currentVertex == b);
        CHECK(state.frontier.size() == 1);

        while(controller.nextStep(state));
        CHECK(state.message == "BFS Finished");
    }
}

TEST_SUITE("AlgorithmController: Dijkstra's algorithm") {
    TEST_CASE("Dijkstra finds and reconstructs the correct path") {
        AdjacencyList<Vertex, Edge> g(false, true);
        int a = g.addVertex(new Vertex("A"));
        int b = g.addVertex(new Vertex("B"));
        int c = g.addVertex(new Vertex("C"));

        g.addEdge(new Edge(g.getVertexById(a), g.getVertexById(c), 10.0));
        g.addEdge(new Edge(g.getVertexById(a), g.getVertexById(b), 2.0));
        g.addEdge(new Edge(g.getVertexById(b), g.getVertexById(c), 2.0));

        AlgorithmController<Vertex, Edge> controller(&g);
        controller.setAlgorithm(AlgorithmType::Dijkstra);
        AlgoState state;

        controller.start(a, c, state);

        while(controller.nextStep(state)) {
            if (state.message.find("Finished") != std::string::npos) break;
        }

        REQUIRE(state.shortestPathEdges.size() == 2);
        CHECK(state.shortestPathEdges[0].from == a);
        CHECK(state.shortestPathEdges[0].to == b);
        CHECK(state.shortestPathEdges[1].from == b);
        CHECK(state.shortestPathEdges[1].to == c);
    }
}

TEST_SUITE("AlgorithmController: history navigation") {
    TEST_CASE("Forward and backward consistency") {
        AdjacencyList<Vertex, Edge> g(false);
        int a = g.addVertex(new Vertex("A"));
        int b = g.addVertex(new Vertex("B"));
        g.addEdge(new Edge(g.getVertexById(a), g.getVertexById(b)));

        AlgorithmController<Vertex, Edge> controller(&g);
        AlgoState state;

        controller.start(a, -1, state);
        controller.nextStep(state);
        std::string messageAtStep1 = state.message;
        int vertexAtStep1 = state.currentVertex;

        controller.nextStep(state);

        bool success = controller.prevStep(state);
        CHECK(success == true);
        CHECK(controller.getCurrentStep() == 1);
        CHECK(state.currentVertex == vertexAtStep1);
        CHECK(state.message == messageAtStep1);
    }
}