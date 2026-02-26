/**
* @file VertexItem.h
 * @brief Graphical representation of a vertex in the scene.
 */

#pragma once
#include <QGraphicsEllipseItem>
#include <QColor>
#include <QString>
#include <QList>

class QGraphicsTextItem;
class EdgeItem;

/**
 * @brief Represents a graphical vertex (node) in the QGraphicsScene.
 * * This class inherits from QGraphicsEllipseItem to visually represent
 * a vertex as a circle. It manages its own label text, an optional distance
 * tracker text (used during algorithm visualization), and tracks connected edges.
 */
class VertexItem : public QGraphicsEllipseItem {
public:
    /**
     * @brief Constructs a VertexItem.
     * @param id The unique identifier of this vertex.
     * @param label The text string to display inside the vertex circle.
     * @param parent The parent QGraphicsItem (default is nullptr).
     */
    explicit VertexItem(int id, const QString& label, QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor. Automatically disconnects from connected EdgeItems upon deletion.
     */
    ~VertexItem() override;

    /**
     * @brief Gets the ID of the vertex.
     * @return The integer ID.
     */
    int getId() const { return m_id; }

    /**
     * @brief Sets or updates the main label text centered within the vertex.
     * @param label The string to display.
     */
    void setLabel(const QString& label);

    /**
     * @brief Changes the fill color of the vertex circle.
     * @param color The new color to apply (e.g., for algorithm state tracking).
     */
    void setColor(const QColor& color);

    /**
     * @brief Restores the fill color of the vertex back to its default state.
     */
    void resetColor();

    /**
     * @brief Displays an accompanying text label showing computed distance.
     * @param text The HTML/plain text representing the distance value.
     */
    void setDistanceText(const QString& text);

    /**
     * @brief Hides the visual distance tracking label.
     */
    void hideDistanceText();

    /**
     * @brief Registers an edge connected to this vertex.
     * @param edge Pointer to the connected EdgeItem.
     */
    void addEdge(EdgeItem* edge);

    /**
     * @brief Unregisters an edge connected to this vertex.
     * @param edge Pointer to the connected EdgeItem.
     */
    void removeEdge(EdgeItem* edge);

    /**
     * @brief Gets a list of all edges currently connected to this vertex.
     * @return A QList of EdgeItem pointers.
     */
    QList<EdgeItem*> getEdges() const { return m_edges; }

    static constexpr int Radius = 20; ///< Defines the graphical radius of the vertex circle.

protected:
    /**
     * @brief Captures changes to the graphics item, such as movement events.
     * @param change The type of change occurring.
     * @param value The value associated with the change.
     * @return Processed value passed back to QGraphicsEllipseItem.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int m_id;                           ///< Internal ID tracking.
    QColor m_defaultColor;              ///< Internal storage for the default vertex color.
    QGraphicsTextItem* m_labelItem;     ///< Graphical item rendering the main ID/name string.
    QGraphicsTextItem* m_distItem;      ///< Graphical item rendering optional distance text.
    QList<EdgeItem*> m_edges;           ///< List of edges anchored to this node.
};