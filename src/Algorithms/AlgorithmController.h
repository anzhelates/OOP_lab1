#pragma once
#include "Algorithm.h"
#include "BFS.h"
#include "DFS.h"
#include "Dijkstra.h"
#include "AlgorithmStep.h"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

namespace Algorithms {

    enum class AlgorithmType {BFS, DFS, Dijkstra};

    template<typename TVertex = Core::Vertex>
    class AlgorithmController {
    public:
        AlgorithmController() = default;

        void setGraph(const Core::Graph<TVertex>* graph) { m_graph = graph; }
        AlgorithmType getAlgorithm() const { return m_type; }
        void setAlgorithm(AlgorithmType type) { m_type = type; }
        int getCurrentStep() const { return m_currentStep; }

        bool start(int startId, int endId, AlgoState& outInitialState) {
            m_states.clear();
            m_currentStep = 0;
            if (!m_graph || !m_graph->hasVertex(startId)) return false;

            AlgoState currentState;
            currentState.distances.resize(m_graph->getVertexCount(), std::numeric_limits<double>::infinity());

            auto vCb = [&](int v, const std::string& state) {
                if (state == "visiting") {
                    currentState.currentVertex = v;
                    auto it = std::find(currentState.frontier.begin(), currentState.frontier.end(), v);
                    if (it != currentState.frontier.end()) currentState.frontier.erase(it);
                } else if (state == "visited") {
                    currentState.visitedVertices.push_back(v);
                    currentState.currentVertex = -1;
                } else if (state == "frontier") {
                    currentState.frontier.push_back(v);
                } else if (state.find("dist=") == 0) {
                    double d = std::stod(state.substr(5));
                    currentState.distances[v] = d;
                }
                m_states.push_back(currentState);
            };

            auto eCb = [&](int from, int to, const std::string& state) {
                if (state == "tree") {
                    currentState.visitedEdges.push_back({from, to});
                } else if (state == "path") {
                    currentState.shortestPathEdges.push_back({from, to});
                }
                m_states.push_back(currentState);
            };

            std::unique_ptr<Algorithm<TVertex>> strategy;
            switch (m_type) {
                case AlgorithmType::BFS: strategy = std::make_unique<BFS<TVertex>>(); break;
                case AlgorithmType::DFS: strategy = std::make_unique<DFS<TVertex>>(); break;
                case AlgorithmType::Dijkstra: strategy = std::make_unique<Dijkstra<TVertex>>(); break;
                default: return false;
            }

            bool success = strategy->run(m_graph, startId, endId, vCb, eCb);
            if (success && !m_states.empty()) {
                outInitialState = m_states.front();
            }
            return success;
        }

        bool nextStep(AlgoState& outState) {
            if (m_currentStep < static_cast<int>(m_states.size()) - 1) {
                m_currentStep++;
                outState = m_states[m_currentStep];
                return true;
            }
            if (!m_states.empty()) outState = m_states.back();
            return false;
        }

        bool prevStep(AlgoState& outState) {
            if (m_currentStep > 0) {
                m_currentStep--;
                outState = m_states[m_currentStep];
                return true;
            }
            if (!m_states.empty()) outState = m_states.front();
            return false;
        }

        void reset() {
            m_states.clear();
            m_currentStep = 0;
        }

    private:
        const Core::Graph<TVertex>* m_graph = nullptr;
        std::vector<AlgoState> m_states;
        int m_currentStep = 0;
        AlgorithmType m_type = AlgorithmType::BFS;
    };
}