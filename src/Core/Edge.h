#pragma once
#include <functional>

namespace Core {

    struct Edge {
        int m_source = -1;
        int m_destination = -1;
        double m_weight = 1.0;
        bool m_active = true;

        Edge() = default;
        Edge(int source, int destination, double weight = 1.0, bool active = true)
            : m_source(source), m_destination(destination), m_weight(weight), m_active(active) {}

        int getSource() const { return m_source; }
        int getDestination() const { return m_destination; }
        double getWeight() const { return m_weight; }

        void markInactive() { m_active = false; }
        void markActive() { m_active = true; }
        bool isActive() const { return m_active; }

        bool operator==(const Edge& other) const {
            return m_source == other.m_source && m_destination == other.m_destination;
        }
    };

    struct EdgeHash {
        std::size_t operator()(const Edge& e) const {
            return std::hash<int>()(e.m_source) ^ (std::hash<int>()(e.m_destination) << 1);
        }
    };
}