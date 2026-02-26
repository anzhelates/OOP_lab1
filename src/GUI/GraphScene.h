/**
* @file GraphScene.h
 * @briefgraphics Scene that manages and displays graph vertices and edges.
 */

#pragma once
#include <QGraphicsScene>
#include <unordered_map>
#include "VertexItem.h"
#include "EdgeItem.h"

/**
 * @brief Hash function object for std::pair, used in unordered containers.
 */
struct PairHash {
    /**
     * @brief Computes the hash value for a given std::pair.
     * @tparam T1 The type of the first element.
     * @tparam T2 The type of the second element.
     * @param p The pair to hash.
     * @return The computed hash value.
     */
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

/**
 * @brief Custom QGraphicsScene subclass that manages graph rendering.
 * * This class handles the creation, storage, and removal of graphical vertices
 * and edges. It acts as the visual representation backend for the graph data.
 */
class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    /**
     * @brief Constructs a GraphScene.
     * @param parent The parent QObject.
     */
    explicit GraphScene(QObject* parent = nullptr);

    /**
     * @brief Destructor. Clears the scene and frees memory.
     */
    ~GraphScene() override;

    /**
     * @brief Clears all graphical elements (vertices and edges) from the scene.
     */
    void clearScene();

    /**
     * @brief Adds a visual vertex to the scene.
     * @param id The unique ID for the vertex.
     * @param label The text label to display inside the vertex.
     * @return Pointer to the newly created VertexItem, or nullptr if it already exists.
     */
    VertexItem* addVertexItem(int id, const QString& label);

    /**
     * @brief Adds a visual edge to the scene.
     * @param fromId The source vertex ID.
     * @param toId The destination vertex ID.
     * @param weight The edge weight.
     * @param isDirected Whether the edge should display an arrowhead.
     * @param isWeighted Whether the edge should display its weight text.
     * @return Pointer to the newly created EdgeItem, or nullptr if creation fails.
     */
    EdgeItem* addEdgeItem(int fromId, int toId, double weight, bool isDirected, bool isWeighted);

    /**
     * @brief Removes a visual vertex from the scene by its ID.
     * @param id The ID of the vertex to remove.
     */
    void removeVertexItem(int id);

    /**
     * @brief Removes a visual edge from the scene by its source and destination IDs.
     * @param fromId The source vertex ID.
     * @param toId The destination vertex ID.
     */
    void removeEdgeItem(int fromId, int toId);

    /**
     * @brief Retrieves a visual vertex by its ID.
     * @param id The target vertex ID.
     * @return Pointer to the corresponding VertexItem, or nullptr if not found.
     */
    VertexItem* getVertexItem(int id) const;

    /**
     * @brief Retrieves a visual edge by its source and destination IDs.
     * @param fromId The source vertex ID.
     * @param toId The destination vertex ID.
     * @return Pointer to the corresponding EdgeItem, or nullptr if not found.
     */
    EdgeItem* getEdgeItem(int fromId, int toId) const;

    /**
     * @brief Applies a calculated layout to update vertex positions.
     * @param positions A map connecting vertex IDs to their newly calculated QPointF coordinates.
     */
    void applyLayout(const QMap<int, QPointF>& positions);

    /**
     * @brief Resets all color highlights applied by algorithm executions to their default states.
     */
    void resetAlgorithmStyles();

private:
    std::unordered_map<int, VertexItem*> m_vertexItems;                        ///< Map storing active VertexItems by ID.
    std::unordered_map<std::pair<int, int>, EdgeItem*, PairHash> m_edgeItems;  ///< Map storing active EdgeItems by ID pairs.

    QPointF m_nextNodePos;                                                     ///< Position tracker for placing new nodes.
    int m_nodesInRow;                                                          ///< Counter to grid-align new nodes.
    static constexpr int MAX_NODES_PER_ROW = 5;                                ///< Maximum nodes allowed per row in grid.
    static constexpr qreal NODE_SPACING = 60.0;                                ///< Distance spacing between newly placed nodes.
};