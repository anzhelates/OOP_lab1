#pragma once

#include <vector>
#include <string>
#include <limits>

struct EdgeId {
    int from = -1;
    int to = -1;

    bool operator<(const EdgeId& other) const {
        if (from != other.from) return from < other.from;
        return to < other.to;
    }
    bool operator==(const EdgeId& other) const {
        return from == other.from && to == other.to;
    }
};

struct AlgoState {
    int currentVertex = -1;
    std::vector<EdgeId> currentEdges;
    std::vector<int> visitedVertices;
    std::vector<EdgeId> visitedEdges;

    std::vector<double> distances;
    std::vector<int> predecessors;

    std::vector<EdgeId> shortestPathEdges;

    std::vector<int> frontier;
    std::string message;
};