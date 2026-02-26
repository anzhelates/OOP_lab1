#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ControlPanel.h"
#include "GraphScene.h"
#include "GraphSetupDialog.h"
#include "AddEdgeDialog.h"
#include "VertexItem.h"
#include "EdgeItem.h"

#include <QPushButton>
#include <QGraphicsView>
#include <QDockWidget>
#include <QMessageBox>
#include <QTimer>
#include <QStandardPaths>
#include <QMenuBar>
#include <QFileDialog>
#include <limits>

using namespace Core;
using namespace Algorithms;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_playSpeedMs(1000)
{
    ui->setupUi(this);

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    m_currentExportDir = desktopPath + QDir::separator() + "ExportedGraphs";

    m_controlPanel = new ControlPanel(this);
    QDockWidget* dockWidget = new QDockWidget("Control Panel", this);
    dockWidget->setWidget(m_controlPanel);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    m_scene = new GraphScene(this);
    m_graphicsView = ui->graphicsView;
    m_graphicsView->setScene(m_scene);
    m_graphicsView->setRenderHint(QPainter::Antialiasing);
    m_graphicsView->setDragMode(QGraphicsView::NoDrag);
    m_graphicsView->setInteractive(true);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_graphicsView->centerOn(0, 0);

    m_algoController = std::make_unique<AlgorithmController<Vertex>>();
    m_playTimer = new QTimer(this);
    m_graphvizEngine = std::make_unique<GraphvizEngine>();

    createMenus();

    connect(m_controlPanel, &ControlPanel::setupGraphButtonClicked, this, &MainWindow::setupGraph);
    connect(m_controlPanel, &ControlPanel::addVertexClicked, this, &MainWindow::addVertex);
    connect(m_controlPanel, &ControlPanel::addEdgeClicked, this, &MainWindow::addEdge);
    connect(m_controlPanel, &ControlPanel::removeVertexClicked, this, &MainWindow::removeVertex);
    connect(m_controlPanel, &ControlPanel::removeEdgeClicked, this, &MainWindow::removeEdge);
    connect(m_controlPanel, &ControlPanel::applyLayoutClicked, this, &MainWindow::onApplyLayout);
    connect(m_controlPanel, &ControlPanel::exportClicked, this, &MainWindow::onExportFromPanel);
    connect(m_controlPanel, &ControlPanel::chooseDirClicked, this, &MainWindow::onSetExportDirectory);

    connect(m_controlPanel, &ControlPanel::algorithmChanged, this, &MainWindow::onAlgorithmChanged);
    connect(m_controlPanel, &ControlPanel::startAlgorithmClicked, this, &MainWindow::onStartAlgorithm);
    connect(m_controlPanel, &ControlPanel::playAlgorithmClicked, this, &MainWindow::onPlayClicked);
    connect(m_controlPanel, &ControlPanel::pauseAlgorithmClicked, this, &MainWindow::onPauseClicked);
    connect(m_controlPanel, &ControlPanel::nextClicked, this, &MainWindow::nextStep);
    connect(m_controlPanel, &ControlPanel::prevClicked, this, &MainWindow::prevStep);
    connect(m_controlPanel, &ControlPanel::resetClicked, this, &MainWindow::resetAlgorithm);
    connect(m_controlPanel, &ControlPanel::speedChanged, this, &MainWindow::onSpeedChanged);
    connect(m_playTimer, &QTimer::timeout, this, &MainWindow::nextStep);

    setCentralWidget(m_graphicsView);
    setWindowTitle("Algorithm Visualiser");
    setupGraph();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createMenus() {
    menuBar()->addMenu(tr("&File"));
}

void MainWindow::createGraph(bool directed, bool weighted) {
    if (m_graph) {
        m_graph->removeObserver(this);
    }

    m_scene->clearScene();
    m_graph = std::make_unique<AdjacencyList<Vertex>>(directed, weighted);

    m_graph->addObserver(this);
    m_algoController->setGraph(m_graph.get());

    updateGraphUI();
    m_controlPanel->setGraphEditingEnabled(true);
    m_controlPanel->setAlgorithmControlsEnabled(false);
    m_controlPanel->setExportEnabled(true);
}

void MainWindow::onVertexAdded(int id) {
    const Vertex* v = m_graph->getVertex(id);
    if (v) m_scene->addVertexItem(id, QString::fromStdString(v->getName()));
}

void MainWindow::onVertexRemoved(int id) {
    m_scene->removeVertexItem(id);
}

void MainWindow::onEdgeAdded(int from, int to, double weight) {
    if (!m_graph->isDirected()) {
        if (m_scene->getEdgeItem(to, from)) return;
    }
    m_scene->addEdgeItem(from, to, weight, m_graph->isDirected(), m_graph->isWeighted());
}

void MainWindow::onEdgeRemoved(int from, int to) {
    m_scene->removeEdgeItem(from, to);
    if (!m_graph->isDirected()) {
        m_scene->removeEdgeItem(to, from);
    }
}

void MainWindow::onGraphCleared() {
    m_scene->clearScene();
}

void MainWindow::setupGraph() {
    onPauseClicked();
    GraphSetupDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        GraphSettings settings = dialog.getSettings();
        createGraph(settings.directed, settings.weighted);

        int vertexCount = dialog.getVertexCount();
        for (int i = 0; i < vertexCount; ++i) {
            m_graph->addVertex(std::make_unique<Vertex>(""));
        }

        auto edges = dialog.getEdges();
        for (const auto& edgeTuple : edges) {
            int fromId = std::get<0>(edgeTuple);
            int toId = std::get<1>(edgeTuple);
            double weight = settings.weighted ? std::get<2>(edgeTuple) : 1.0;

            if (fromId != toId && m_graph->hasVertex(fromId) && m_graph->hasVertex(toId) && !m_graph->hasEdge(fromId, toId)) {
                m_graph->addEdge(fromId, toId, weight);
            }
        }
        updateGraphUI();
        onApplyLayout();
    } else if (!m_graph) {
        createGraph(false, false);
    }
}

void MainWindow::addVertex() {
    onPauseClicked();
    if (!m_graph) return;

    m_graph->addVertex(std::make_unique<Vertex>(""));

    updateGraphUI();
    resetAlgorithm();
    onApplyLayout();
}

void MainWindow::addEdge() {
    onPauseClicked();
    if (!m_graph) return;

    int maxActiveId = -1;
    int activeVertices = 0;
    for (const auto* v : m_graph->getVertices()) {
        if (v && v->isActive()) {
            activeVertices++;
            if (v->getId() > maxActiveId) maxActiveId = v->getId();
        }
    }

    if (activeVertices < 2) {
        QMessageBox::warning(this, "Error", "At least 2 active vertices are required.");
        return;
    }

    AddEdgeDialog dialog(m_graph->isWeighted(), maxActiveId, this);
    if (dialog.exec() == QDialog::Accepted) {
        int fromId = dialog.getFromId();
        int toId = dialog.getToId();

        if (!m_graph->hasVertex(fromId) || !m_graph->hasVertex(toId)) {
            QMessageBox::warning(this, "Error", "Invalid vertex ID.");
            return;
        }
        if (fromId == toId) {
            QMessageBox::warning(this, "Error", "Loops are not allowed.");
            return;
        }
        if (m_graph->hasEdge(fromId, toId) || (!m_graph->isDirected() && m_graph->hasEdge(toId, fromId))) {
            QMessageBox::warning(this, "Error", "This edge already exists.");
            return;
        }

        double weight = m_graph->isWeighted() ? dialog.getWeight() : 1.0;
        m_graph->addEdge(fromId, toId, weight);

        updateGraphUI();
        resetAlgorithm();
        onApplyLayout();
    }
}

void MainWindow::removeVertex() {
    onPauseClicked();
    if (!m_graph) return;

    VertexItem* selectedItem = nullptr;
    for (QGraphicsItem* item : m_scene->selectedItems()) {
        selectedItem = qgraphicsitem_cast<VertexItem*>(item);
        if (selectedItem) break;
    }

    if (!selectedItem) {
        QMessageBox::information(this, "Remove", "Select a vertex to remove.");
        return;
    }

    m_graph->removeVertex(selectedItem->getId());
    updateGraphUI();
    resetAlgorithm();
    onApplyLayout();
}

void MainWindow::removeEdge() {
    onPauseClicked();
    if (!m_graph) return;

    EdgeItem* selectedItem = nullptr;
    for (QGraphicsItem* item : m_scene->selectedItems()) {
        selectedItem = qgraphicsitem_cast<EdgeItem*>(item);
        if (selectedItem) break;
    }

    if (!selectedItem) {
        QMessageBox::information(this, "Remove", "Select an edge to remove.");
        return;
    }

    m_graph->removeEdge(selectedItem->getFromId(), selectedItem->getToId());
    updateGraphUI();
    resetAlgorithm();
    onApplyLayout();
}

void MainWindow::updateGraphUI() {
    if (!m_graph) return;

    m_controlPanel->updateStartVertexComboBox(m_graph->getVertices());

    bool hasActiveVertices = false;
    for (const auto* v : m_graph->getVertices()) {
        if (v && v->isActive()) {
            hasActiveVertices = true;
            break;
        }
    }
    m_controlPanel->setAlgorithmControlsEnabled(hasActiveVertices);
}

void MainWindow::onAlgorithmChanged(AlgorithmType type) {
    if (type == AlgorithmType::Dijkstra && m_graph && !m_graph->isWeighted()) {
        QMessageBox::information(this, "Info", "Dijkstra's algorithm is better used on a weighted graph.");
    }
    m_algoController->setAlgorithm(type);
    resetAlgorithm();
}

void MainWindow::onStartAlgorithm(int startVertexId, int endVertexId) {
    onPauseClicked();
    resetAlgorithmStyles();
    m_algoController->reset();

    if (m_algoController->start(startVertexId, endVertexId, m_currentState)) {
        applyState(m_currentState);
        m_controlPanel->getPrevButton()->setEnabled(false);
        m_controlPanel->getNextButton()->setEnabled(true);
        m_controlPanel->getPlayButton()->setEnabled(true);
    } else {
        QMessageBox::warning(this, "Error", "Algorithm failed to start.");
        resetAlgorithm();
    }
}

void MainWindow::nextStep() {
    if (m_algoController->nextStep(m_currentState)) {
        applyState(m_currentState);
        m_controlPanel->getPrevButton()->setEnabled(true);
    } else {
        if (m_playTimer->isActive()) onPauseClicked();
        applyState(m_currentState);

        m_controlPanel->getNextButton()->setEnabled(false);
        m_controlPanel->getPlayButton()->setEnabled(false);
        QMessageBox::information(this, "Finished", "Algorithm completed.");
    }
}

void MainWindow::prevStep() {
    if (m_playTimer->isActive()) onPauseClicked();

    if (m_algoController->prevStep(m_currentState)) {
        applyState(m_currentState);
        m_controlPanel->getNextButton()->setEnabled(true);
        m_controlPanel->getPlayButton()->setEnabled(true);
    }
    m_controlPanel->getPrevButton()->setEnabled(m_algoController->getCurrentStep() > 0);
}

void MainWindow::resetAlgorithm() {
    onPauseClicked();
    m_algoController->reset();
    resetAlgorithmStyles();
    m_currentState = AlgoState();
    m_controlPanel->resetPlayerControls();
    updateGraphUI();
}

void MainWindow::onPlayClicked() {
    if (m_algoController->getCurrentStep() == 0 && m_controlPanel->getStartButton()->isEnabled()) {
         m_controlPanel->getStartButton()->click();
         if(m_algoController->getCurrentStep() == 0) return;
    }
    m_playTimer->start(m_playSpeedMs);
    m_controlPanel->getPlayButton()->setEnabled(false);
    m_controlPanel->getPauseButton()->setEnabled(true);
}

void MainWindow::onPauseClicked() {
    if (m_playTimer->isActive()) {
        m_playTimer->stop();
        m_controlPanel->getPlayButton()->setEnabled(true);
        m_controlPanel->getPauseButton()->setEnabled(false);
        m_controlPanel->getNextButton()->setEnabled(true);
        m_controlPanel->getPrevButton()->setEnabled(m_algoController->getCurrentStep() > 0);
    }
}

void MainWindow::onSpeedChanged(int speed) {
    m_playSpeedMs = speed;
    if (m_playTimer->isActive()) {
        m_playTimer->start(m_playSpeedMs);
    }
}

void MainWindow::applyState(const AlgoState& state) {
    resetAlgorithmStyles();

    const QColor visitedColor(Qt::darkGray);
    const QColor frontierColor(QColor(255, 165, 0));
    const QColor processingColor(Qt::yellow);
    const QColor treeEdgeColor(QColor(0, 100, 0));
    const QColor pathColor(Qt::magenta);

    bool isDijkstra = (m_algoController->getAlgorithm() == AlgorithmType::Dijkstra);

    if (isDijkstra && !state.distances.empty()) {
        for (const auto* v : m_graph->getVertices()) {
            if (v && v->isActive() && v->getId() < static_cast<int>(state.distances.size())) {
                VertexItem* vItem = m_scene->getVertexItem(v->getId());
                if (vItem) {
                    double dist = state.distances[v->getId()];
                    if (dist == std::numeric_limits<double>::infinity()) {
                        vItem->setDistanceText("<b>&infin;</b>");
                    } else {
                        vItem->setDistanceText(QString("<b>%1</b>").arg(QString::number(dist, 'f', 1)));
                    }
                }
            }
        }
    }

    for (const EdgeId& id : state.visitedEdges) {
        EdgeItem* eItem = m_scene->getEdgeItem(id.from, id.to);
        if (!eItem && !m_graph->isDirected()) eItem = m_scene->getEdgeItem(id.to, id.from);
        if (eItem) eItem->setColor(treeEdgeColor);
    }

    for (int vertexId : state.visitedVertices) {
        VertexItem* vItem = m_scene->getVertexItem(vertexId);
        if (vItem) vItem->setColor(visitedColor);
    }

    for (int vertexId : state.frontier) {
        VertexItem* vItem = m_scene->getVertexItem(vertexId);
        if (vItem && vItem->brush().color() != visitedColor) {
            vItem->setColor(frontierColor);
        }
    }

    if (state.currentVertex >= 0) {
        VertexItem* vItem = m_scene->getVertexItem(state.currentVertex);
        if (vItem) vItem->setColor(processingColor);
    }

    for (const EdgeId& id : state.shortestPathEdges) {
        EdgeItem* eItem = m_scene->getEdgeItem(id.from, id.to);
        if (!eItem && !m_graph->isDirected()) eItem = m_scene->getEdgeItem(id.to, id.from);
        if (eItem) {
            eItem->setColor(pathColor);
            eItem->setZValue(2);
        }
        VertexItem* v1 = m_scene->getVertexItem(id.from);
        VertexItem* v2 = m_scene->getVertexItem(id.to);
        if (v1) { v1->setColor(pathColor); v1->setZValue(3); }
        if (v2) { v2->setColor(pathColor); v2->setZValue(3); }
    }
}

void MainWindow::resetAlgorithmStyles() {
    m_scene->resetAlgorithmStyles();
}

void MainWindow::onApplyLayout() {
    if (!m_graph) return;
    onPauseClicked();
    resetAlgorithm();

    auto algo = GraphvizEngine::getDefaultLayoutAlgorithm(m_graph->isDirected());
    QMap<int, QPointF> positions = m_graphvizEngine->layoutGraph(m_graph.get(), algo);

    m_scene->applyLayout(positions);
    QRectF rect = m_scene->itemsBoundingRect();
    rect.adjust(-150, -150, 150, 150);
    m_scene->setSceneRect(rect);
    m_graphicsView->fitInView(rect, Qt::KeepAspectRatio);
}

void MainWindow::onSetExportDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Export Directory"),
        m_currentExportDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) m_currentExportDir = dir;
}

void MainWindow::onExportFromPanel(const QString& format) {
    if (!m_graph) return;

    QString defaultExt;
    QString formatId = format.toLower();

    if (formatId == "png") defaultExt = ".png";
    else if (formatId == "svg") defaultExt = ".svg";
    else if (formatId == "dot") defaultExt = ".dot";
    else if (formatId == "pdf") defaultExt = ".pdf";
    else if (formatId == "json") defaultExt = ".json";
    else if (formatId == "jpeg") defaultExt = ".jpeg";
    else return;

    QDir outputDir(m_currentExportDir);
    if (!outputDir.exists() && !outputDir.mkpath(".")) {
        QMessageBox::warning(this, "Error", "Cannot create folder: " + m_currentExportDir);
        return;
    }

    QString baseName = "graph";
    QString finalName = baseName + defaultExt;
    int counter = 1;

    while (outputDir.exists(finalName)) {
        finalName = QString("%1 (%2)%3").arg(baseName).arg(counter).arg(defaultExt);
        counter++;
    }

    QString filePath = outputDir.absoluteFilePath(finalName);
    auto algo = GraphvizEngine::getDefaultLayoutAlgorithm(m_graph->isDirected());

    if (m_graphvizEngine->exportToFile(m_graph.get(), filePath, formatId, algo)) {
        QMessageBox::information(this, "Success", "Graph saved to:\n" + filePath);
    } else {
        QMessageBox::warning(this, "Error", "Failed to save file:\n" + filePath);
    }
}