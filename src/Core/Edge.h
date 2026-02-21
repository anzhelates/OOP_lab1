#pragma once

#include "Vertex.h"

class Edge {
private:
    Vertex* m_source;
    Vertex* m_destination;
    double m_weight;
    bool m_active;

public:
    Edge(Vertex* source, Vertex* destination, double weight = 1.0, bool active = true)
    : m_source(source), m_destination(destination), m_weight(weight), m_active(active) {}
    virtual ~Edge() = default;

    Vertex* getSource() const { return m_source; }
    Vertex* getDestination() const { return m_destination; }
    double getWeight() const { return m_weight; }
    void setWeight(double weight) { m_weight = weight; }
    int getFrom() const { return m_source ? m_source->getId() : -1; }
    int getTo() const { return m_destination ? m_destination->getId() : -1; }

    void markInactive() { m_active = false; }
    void markActive() { m_active = true; }
    bool isActive() const { return m_active; }
};