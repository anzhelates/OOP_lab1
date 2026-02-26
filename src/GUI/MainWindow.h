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

class MainWindow : public QMainWindow, public Core::GraphObserver {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void onVertexAdded(int id) override;
    void onVertexRemoved(int id) override;
    void onEdgeAdded(int from, int to, double weight) override;
    void onEdgeRemoved(int from, int to) override;
    void onGraphCleared() override;

private slots:
    void setupGraph();
    void addVertex();
    void addEdge();
    void removeVertex();
    void removeEdge();
    void onAlgorithmChanged(Algorithms::AlgorithmType type);
    void onStartAlgorithm(int startVertexId, int endVertexId);

    void nextStep();
    void prevStep();
    void resetAlgorithm();
    void onPlayClicked();
    void onPauseClicked();
    void onSpeedChanged(int speed);
    void onApplyLayout();
    void onExportFromPanel(const QString& format);
    void onSetExportDirectory();

private:
    void createGraph(bool directed, bool weighted);
    void updateGraphUI();
    void applyState(const AlgoState& state);
    void resetAlgorithmStyles();
    void createMenus();

    Ui::MainWindow* ui;
    ControlPanel* m_controlPanel;
    QGraphicsView* m_graphicsView;
    GraphScene* m_scene;

    std::unique_ptr<Core::AdjacencyList<Core::Vertex>> m_graph;
    std::unique_ptr<Algorithms::AlgorithmController<Core::Vertex>> m_algoController;
    std::unique_ptr<GraphvizEngine> m_graphvizEngine;

    AlgoState m_currentState;
    QTimer* m_playTimer;
    int m_playSpeedMs;
    QString m_currentExportDir;
};