#pragma once

#include <string>
#include <vector>

template <typename TVertex, typename TEdge> class Graph;
template <typename TVertex, typename TEdge> class AdjacencyList;
template <typename TVertex, typename TEdge> class AdjacencyMatrix;

class Vertex {
private:
    int m_id = -1;
    std::string m_name;
    bool m_active = true;

protected:
    void setId(int id) { m_id = id; }

public:
    Vertex() = default;
    explicit Vertex(const std::string& name) : m_name(name) {}
    virtual ~Vertex() = default;

    int getId() const { return m_id; }
    bool isActive() const { return m_active; }
    void markInactive() { m_active = false; }
    void markActive() { m_active = true; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    template <typename TVertex, typename TEdge> friend class Graph;
    template <typename TVertex, typename TEdge> friend class AdjacencyList;
    template <typename TVertex, typename TEdge> friend class AdjacencyMatrix;
};