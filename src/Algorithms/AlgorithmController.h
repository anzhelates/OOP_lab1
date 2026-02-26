/**
* @file AlgorithmController.h
 * @brief Manages execution and step-by-step control of graph algorithms.
 */

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

    /**
     * @brief Enumeration of available graph algorithms.
     */
    enum class AlgorithmType {BFS, DFS, Dijkstra};

    /**
     * @brief Controls the execution and state management of graph algorithms.
     * * This class acts as a facade to run algorithms, collect their execution steps,
     * and allow stepping through the states backwards and forwards.
     * * @tparam TVertex The vertex type used in the graph. Defaults to Core::Vertex.
     */
    template<typename TVertex = Core::Vertex>
    class AlgorithmController {
    public:
        /**
         * @brief Default constructor.
         */
        AlgorithmController() = default;

        /**
         * @brief Sets the graph instance to be used by the algorithms.
         * * @param graph Pointer to the target graph.
         */
        void setGraph(const Core::Graph<TVertex>* graph) { m_graph = graph; }

        /**
         * @brief Gets the currently selected algorithm type.
         * * @return AlgorithmType The current algorithm type.
         */
        AlgorithmType getAlgorithm() const { return m_type; }

        /**
         * @brief Sets the algorithm type to be executed.
         * * @param type The desired algorithm type.
         */
        void setAlgorithm(AlgorithmType type) { m_type = type; }

        /**
         * @brief Gets the current step index of the algorithm's execution history.
         * * @return int The current step index.
         */
        int getCurrentStep() const { return m_currentStep; }

        /**
         * @brief Starts the execution of the selected algorithm.
         * * This method runs the algorithm entirely and records its states at every step using callbacks.
         * * @param startId The ID of the starting vertex.
         * @param endId The ID of the target vertex.
         * @param outInitialState Reference to an AlgoState variable that will receive the very first recorded state.
         * @return true If the algorithm was successfully executed and at least one state was recorded.
         * @return false If the setup is invalid or execution failed.
         */
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

        /**
         * @brief Advances to the next recorded state of the algorithm.
         * * @param outState Reference to an AlgoState variable that will receive the next state.
         * @return true If moved to the next step successfully.
         * @return false If already at the last step.
         */
        bool nextStep(AlgoState& outState) {
            if (m_currentStep < static_cast<int>(m_states.size()) - 1) {
                m_currentStep++;
                outState = m_states[m_currentStep];
                return true;
            }
            if (!m_states.empty()) outState = m_states.back();
            return false;
        }

        /**
         * @brief Retreats to the previous recorded state of the algorithm.
         * * @param outState Reference to an AlgoState variable that will receive the previous state.
         * @return true If moved to the previous step successfully.
         * @return false If already at the first step.
         */
        bool prevStep(AlgoState& outState) {
            if (m_currentStep > 0) {
                m_currentStep--;
                outState = m_states[m_currentStep];
                return true;
            }
            if (!m_states.empty()) outState = m_states.front();
            return false;
        }

        /**
         * @brief Resets the controller, clearing all recorded states and resetting the current step.
         */
        void reset() {
            m_states.clear();
            m_currentStep = 0;
        }

    private:
        const Core::Graph<TVertex>* m_graph = nullptr; ///< Pointer to the graph instance.
        std::vector<AlgoState> m_states;               ///< Collection of all states recorded during algorithm execution.
        int m_currentStep = 0;                         ///< The current index within the recorded states.
        AlgorithmType m_type = AlgorithmType::BFS;     ///< The selected algorithm type.
    };
}