/**
* @file GraphvizEngine.h
 * @brief Component responsible for generating graph layouts using Graphviz and exporting graphs in various formats.
 */

#pragma once
#include "Graph.h"
#include "Vertex.h"
#include <QMap>
#include <QPointF>
#include <QString>
#include <string>

/**
 * @brief Forward declarations for Graphviz C structs
 */
struct Agraph_s;
struct GVC_s;
typedef struct Agraph_s Agraph_t;
typedef struct GVC_s GVC_t;

/**
 * @brief Wrapper engine around the Graphviz C library.
 * * Provides functionalities to calculate automatic graph layouts
 * and export graphs to various image and data formats.
 */
class GraphvizEngine {
public:
    /**
     * @brief Enumeration of supported layout algorithms provided by Graphviz.
     */
    enum class LayoutAlgorithm {
        DOT,   ///< Hierarchical layout, best for directed graphs.
        NEATO  ///< Spring model layout, best for undirected graphs.
    };

    /**
     * @brief Constructor. Initializes the underlying Graphviz context.
     */
    GraphvizEngine();

    /**
     * @brief Destructor. Frees the Graphviz context memory.
     */
    ~GraphvizEngine();

    /**
     * @brief Deleted copy constructor to prevent context duplication.
     */
    GraphvizEngine(const GraphvizEngine&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent context duplication.
     */
    GraphvizEngine& operator=(const GraphvizEngine&) = delete;

    /**
     * @brief Calculates a visual layout for the provided graph using Graphviz.
     * @param graph The underlying data model of the graph.
     * @param algo The Graphviz layout algorithm to execute.
     * @return A map linking each vertex ID to its calculated QPointF spatial coordinate.
     */
    QMap<int, QPointF> layoutGraph(const Core::Graph<Core::Vertex>* graph, LayoutAlgorithm algo);

    /**
     * @brief Exports the graph diagram to an external file using Graphviz rendering.
     * @param graph The underlying data model of the graph.
     * @param filePath The destination file path.
     * @param format The output format (e.g., "png", "svg", "pdf").
     * @param algo The layout algorithm to use prior to rendering.
     * @return True if the export was successful, false otherwise.
     */
    bool exportToFile(const Core::Graph<Core::Vertex>* graph, const QString& filePath, const QString& format, LayoutAlgorithm algo);

    /**
     * @brief Recommends an optimal layout algorithm based on whether the graph is directed.
     * @param directed True if the graph is directed.
     * @return The recommended LayoutAlgorithm (DOT for directed, NEATO for undirected).
     */
    static LayoutAlgorithm getDefaultLayoutAlgorithm(bool directed);

private:
    /**
     * @brief Converts the internal C++ graph structure to a Graphviz Agraph_t representation.
     * @param graph The underlying data model of the graph.
     * @return Pointer to the populated Graphviz graph structure.
     */
    Agraph_t* createGvGraph(const Core::Graph<Core::Vertex>* graph);

    /**
     * @brief Retrieves the string representation of a given layout enum.
     * @param algo The LayoutAlgorithm enum value.
     * @return The corresponding string compatible with the Graphviz API (e.g., "dot", "neato").
     */
    std::string getLayoutAlgoString(LayoutAlgorithm algo);

    GVC_t* m_gvc; ///< The Graphviz rendering context.
};