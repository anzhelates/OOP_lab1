#pragma once

#include <QMainWindow>
#include <memory>
#include <QDir>
#include "AlgorithmStep.h"
#include "AlgorithmController.h"
#include "Graph.h"
#include "GraphvizEngine.h"

enum class AlgorithmType;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsView;
class ControlPanel;
class GraphScene;
class Vertex;
class Edge;
class QTimer;
class QMenu;
class QAction;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    private slots:
        void setupGraph();
        void addVertex();
        void addEdge();
        void removeVertex();
        void removeEdge();
        void onAlgorithmChanged(AlgorithmType type);
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
        void exportToDot();
        void exportToPng();
        void exportToSvg();
        void exportToPdf();
        void exportToJson();
        void exportToJpeg();

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

    std::unique_ptr<Graph<Vertex, Edge>> m_graph;
    std::unique_ptr<AlgorithmController<Vertex, Edge>> m_algoController;
    std::unique_ptr<GraphvizEngine> m_graphvizEngine;

    AlgoState m_currentState;

    QTimer* m_playTimer;
    int m_playSpeedMs;
    QString m_currentExportDir;
    QMenu* m_fileMenu;
    QAction* m_exportDotAction;
    QAction* m_exportPngAction;
    QAction* m_exportSvgAction;
};