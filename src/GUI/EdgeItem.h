/**
* @file EdgeItem.h
 * @brief Graphical representation of an edge in the scene.
 */

#pragma once
#include <QGraphicsPathItem>
#include <QPen>

class VertexItem;
class QGraphicsTextItem;

/**
 * @brief Represents a graphical edge between two vertices in a QGraphicsScene.
 * * This class inherits from QGraphicsPathItem to draw a connecting line
 * (and optionally an arrowhead for directed graphs) between a source
 * and a destination VertexItem. It also manages the graphical text
 * representation of the edge's weight.
 */
class EdgeItem : public QGraphicsPathItem {
public:
    /**
     * @brief Constructs an EdgeItem.
     * @param fromId The ID of the source vertex.
     * @param toId The ID of the destination vertex.
     * @param source Pointer to the source VertexItem.
     * @param dest Pointer to the destination VertexItem.
     * @param weight The weight of the edge.
     * @param directed True if the edge is directed (draws an arrow).
     * @param weighted True if the edge is weighted (draws the weight text).
     * @param parent The parent QGraphicsItem (default is nullptr).
     */
    EdgeItem(int fromId, int toId, VertexItem* source, VertexItem* dest, double weight, bool directed, bool weighted, QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor. Removes the edge references from the connected source and destination nodes.
     */
    ~EdgeItem() override;

    /**
     * @brief Gets the ID of the source vertex.
     * @return The source vertex ID.
     */
    int getFromId() const { return m_fromId; }

    /**
     * @brief Gets the ID of the destination vertex.
     * @return The destination vertex ID.
     */
    int getToId() const { return m_toId; }

    /**
     * @brief Gets a pointer to the source VertexItem.
     * @return Pointer to the source node.
     */
    VertexItem* sourceNode() const { return m_source; }

    /**
     * @brief Gets a pointer to the destination VertexItem.
     * @return Pointer to the destination node.
     */
    VertexItem* destNode() const { return m_dest; }

    /**
     * @brief Clears the reference to the source node, safely detaching it.
     */
    void clearSource() { m_source = nullptr; }

    /**
     * @brief Clears the reference to the destination node, safely detaching it.
     */
    void clearDest() { m_dest = nullptr; }

    /**
     * @brief Recalculates and updates the graphical position of the edge based on its connected nodes.
     */
    void updatePosition();

    /**
     * @brief Sets the weight of the edge and updates its visual text representation.
     * @param weight The new weight value.
     */
    void setWeight(double weight);

    /**
     * @brief Changes the color of the edge (e.g., to highlight during algorithm execution).
     * @param color The new color to apply.
     */
    void setColor(const QColor& color);

    /**
     * @brief Resets the edge color back to its default state.
     */
    void resetColor();

    /**
     * @brief Calculates the bounding rectangle of the edge item.
     * @return The bounding rectangle as a QRectF.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Defines the precise shape of the edge for collision detection and selection.
     * @return The shape as a QPainterPath.
     */
    QPainterPath shape() const override;

    /**
     * @brief Custom paint routine for drawing the edge, including selection states and arrowheads.
     * @param painter The QPainter used for drawing.
     * @param option Style options for the item.
     * @param widget The widget being painted on.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int m_fromId;                           ///< Internal storage for the source ID.
    int m_toId;                             ///< Internal storage for the destination ID.
    double m_weight;                        ///< Internal storage for the edge weight.
    bool m_directed;                        ///< Internal flag indicating if the edge is directed.
    bool m_weighted;                        ///< Internal flag indicating if the edge is weighted.

    VertexItem* m_source;                   ///< Pointer to the source visual node.
    VertexItem* m_dest;                     ///< Pointer to the destination visual node.
    QGraphicsTextItem* m_weightText;        ///< Graphical item displaying the weight text.
    QColor m_defaultColor;                  ///< The default color of the edge line.
};