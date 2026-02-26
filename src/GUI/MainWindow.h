/**
 * @file MainWindow.h
 * @brief Defines the MainWindow class which serves as the primary user interface.
 */

#pragma once
#include <QMainWindow>
#include <memory>
#include <QDir>
#include <QString>
#include "AlgorithmController.h"
#include "AlgorithmStep.h"
#include "GraphObserver.h"
#include "Graph.h"
#include "AdjacencyList.h"
#include "GraphvizEngine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsView;
class ControlPanel;
class GraphScene;
class QTimer;
class QMenu;
class QAction;

/**
 * @class MainWindow
 * @brief The main application window that orchestrates the UI, graph visualization, and algorithm execution.
 * * This class inherits from QMainWindow to provide the main layout and from Core::GraphObserver
 * to react to changes within the graph data structure and update the UI accordingly.
 */
class MainWindow : public QMainWindow, public Core::GraphObserver {
    Q_OBJECT
public:
    /**
     * @brief Constructs the MainWindow object.
     * @param parent The parent widget, if any (defaults to nullptr).
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destroys the MainWindow object and cleans up UI resources.
     */
    ~MainWindow() override;

    /**
     * @brief Callback invoked when a vertex is added to the graph.
     * @param id The unique identifier of the added vertex.
     */
    void onVertexAdded(int id) override;

    /**
     * @brief Callback invoked when a vertex is removed from the graph.
     * @param id The unique identifier of the removed vertex.
     */
    void onVertexRemoved(int id) override;

    /**
     * @brief Callback invoked when an edge is added to the graph.
     * @param from The ID of the source vertex.
     * @param to The ID of the destination vertex.
     * @param weight The weight of the newly added edge.
     */
    void onEdgeAdded(int from, int to, double weight) override;

    /**
     * @brief Callback invoked when an edge is removed from the graph.
     * @param from The ID of the source vertex.
     * @param to The ID of the destination vertex.
     */
    void onEdgeRemoved(int from, int to) override;

    /**
     * @brief Callback invoked when the graph is entirely cleared.
     */
    void onGraphCleared() override;

private slots:
    /**
     * @brief Opens a dialog to setup initial graph properties and applies them.
     */
    void setupGraph();

    /**
     * @brief Adds a new vertex to the currently active graph.
     */
    void addVertex();

    /**
     * @brief Opens a dialog to add a new edge between existing vertices.
     */
    void addEdge();

    /**
     * @brief Removes the currently selected vertex from the graph and scene.
     */
    void removeVertex();

    /**
     * @brief Removes the currently selected edge from the graph and scene.
     */
    void removeEdge();

    /**
     * @brief Handles the change of the selected algorithm.
     * @param type The type of algorithm chosen by the user.
     */
    void onAlgorithmChanged(Algorithms::AlgorithmType type);

    /**
     * @brief Starts the execution of the currently selected algorithm.
     * @param startVertexId The ID of the starting vertex.
     * @param endVertexId The ID of the target vertex (if applicable).
     */
    void onStartAlgorithm(int startVertexId, int endVertexId);

    /**
     * @brief Advances the algorithm visualization by one step.
     */
    void nextStep();

    /**
     * @brief Reverts the algorithm visualization by one step.
     */
    void prevStep();

    /**
     * @brief Resets the algorithm state and visualization back to its initial state.
     */
    void resetAlgorithm();

    /**
     * @brief Starts playing the algorithm visualization automatically.
     */
    void onPlayClicked();

    /**
     * @brief Pauses the automatic playback of the algorithm visualization.
     */
    void onPauseClicked();

    /**
     * @brief Adjusts the playback speed of the algorithm visualization.
     * @param speed The new speed in milliseconds between steps.
     */
    void onSpeedChanged(int speed);

    /**
     * @brief Applies an automatic layout to the graph visually using Graphviz.
     */
    void onApplyLayout();

    /**
     * @brief Exports the current graph to a file from the control panel.
     * @param format The desired export format (e.g., png, svg, dot).
     */
    void onExportFromPanel(const QString& format);

    /**
     * @brief Opens a dialog to set the directory where exported graphs will be saved.
     */
    void onSetExportDirectory();

private:
    /**
     * @brief Recreates the internal graph structure.
     * @param directed True if the graph should be directed.
     * @param weighted True if the graph should support edge weights.
     */
    void createGraph(bool directed, bool weighted);

    /**
     * @brief Updates the user interface elements depending on the current graph state.
     */
    void updateGraphUI();

    /**
     * @brief Applies a specific algorithm state to the visualization, coloring nodes and edges.
     * @param state The state data to be visually applied.
     */
    void applyState(const AlgoState& state);

    /**
     * @brief Resets all visual styling changes made by the algorithm back to default.
     */
    void resetAlgorithmStyles();

    /**
     * @brief Creates the main window menus.
     */
    void createMenus();

    Ui::MainWindow* ui;                                                     ///< Pointer to the UI file configuration.
    ControlPanel* m_controlPanel;                                           ///< Pointer to the control panel dock widget.
    QGraphicsView* m_graphicsView;                                          ///< Pointer to the graphics view displaying the graph.
    GraphScene* m_scene;                                                    ///< Pointer to the custom QGraphicsScene managing graph items.

    std::unique_ptr<Core::AdjacencyList<Core::Vertex>> m_graph;             ///< Underlying graph data structure.
    std::unique_ptr<Algorithms::AlgorithmController<Core::Vertex>> m_algoController; ///< Controller managing algorithm steps.
    std::unique_ptr<GraphvizEngine> m_graphvizEngine;                       ///< Engine for graph layout and exporting.

    AlgoState m_currentState;                                               ///< The current step state of an executing algorithm.
    QTimer* m_playTimer;                                                    ///< Timer governing the automatic algorithm playback.
    int m_playSpeedMs;                                                      ///< Current interval between algorithm steps in milliseconds.
    QString m_currentExportDir;                                             ///< Directory where graph exports are saved.
};