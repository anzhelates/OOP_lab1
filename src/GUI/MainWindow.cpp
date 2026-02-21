#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ControlPanel.h"
#include "GraphScene.h"
#include "GraphSetupDialog.h"
#include "AddEdgeDialog.h"

#include "AdjacencyList.h"
#include "Vertex.h"
#include "Edge.h"
#include "VertexItem.h"
#include "EdgeItem.h"
#include "GraphvizEngine.h"

#include <QGraphicsView>
#include <QDockWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <limits>
#include <set>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

using MyGraph = AdjacencyList<Vertex, Edge>;
using MyAlgoController = AlgorithmController<Vertex, Edge>;

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

    m_algoController = std::make_unique<MyAlgoController>(nullptr);
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
    m_fileMenu = menuBar()->addMenu(tr("&File"));
}

void MainWindow::createGraph(bool directed, bool weighted) {
    m_scene->clearScene();
    m_graph.reset();
    m_algoController->setGraph(nullptr);

    m_graph = std::make_unique<MyGraph>(directed, weighted);

    m_scene->setGraph(m_graph.get());
    m_algoController->setGraph(m_graph.get());

    updateGraphUI();
    m_controlPanel->setGraphEditingEnabled(true);
    m_controlPanel->setAlgorithmControlsEnabled(false);

    bool enabled = (m_graph != nullptr);
    m_controlPanel->setExportEnabled(enabled);
}

void MainWindow::setupGraph() {
    onPauseClicked();

    GraphSetupDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        GraphSettings settings = dialog.getSettings();
        createGraph(settings.directed, settings.weighted);

        int vertexCount = dialog.getVertexCount();
        for (int i = 0; i < vertexCount; ++i) {
            std::string name = "v" + std::to_string(i);
            Vertex* newVertex = new Vertex(name);
            int id = m_graph->addVertex(newVertex);
            if (id != -1) {
                m_scene->addVertex(newVertex);
            } else {
                delete newVertex;
            }
        }

        auto edges = dialog.getEdges();
        for (const auto& edgeTuple : edges) {
            int fromId = std::get<0>(edgeTuple);
            int toId = std::get<1>(edgeTuple);
            double weight = m_graph->isWeighted() ? std::get<2>(edgeTuple) : 1.0;

            Vertex* v_from = m_graph->getVertexById(fromId);
            Vertex* v_to = m_graph->getVertexById(toId);

            if (v_from && v_to) {
                bool exists = m_graph->getEdge(fromId, toId) != nullptr;
                if (!m_graph->isDirected()) {
                    exists = exists || (m_graph->getEdge(toId, fromId) != nullptr);
                }
                if (exists) {
                    qWarning() << "Skipping duplicate edge from dialog:" << fromId << "->" << toId;
                    continue;
                }
                Edge* newEdge = new Edge(v_from, v_to, weight);
                m_graph->addEdge(newEdge);
                m_scene->addEdge(newEdge);
            } else {
                qWarning() << "Invalid edge from dialog" << fromId << "->" << toId;
            }
        }
        updateGraphUI();
        m_scene->setSceneRect(m_scene->itemsBoundingRect());
        onApplyLayout();

    } else {
        if (!m_graph) {
            createGraph(false, false);
        }
    }
}

void MainWindow::addVertex() {
    onPauseClicked();
    if (!m_graph) return;

    std::string name = "v" + std::to_string(m_graph->getVertexCount());
    Vertex* newVertex = new Vertex(name);

    int id = m_graph->addVertex(newVertex);
    if (id == -1) {
        delete newVertex;
        return;
    }

    newVertex->setName("v" + std::to_string(id));
    m_scene->addVertex(newVertex);

    updateGraphUI();
    resetAlgorithm();
}

void MainWindow::addEdge() {
    onPauseClicked();
    if (!m_graph) return;

    int maxActiveId = -1;
    int activeVertices = 0;
     for(auto v : m_graph->getVertices()) {
        if (v && v->isActive()) {
            activeVertices++;
            if(v->getId() > maxActiveId) {
                maxActiveId = v->getId();
            }
        }
    }

    if (activeVertices < 2) {
        QMessageBox::warning(this, "Error", "At least two active vertices needed");
        return;
    }

    AddEdgeDialog dialog(m_graph->isWeighted(), maxActiveId, this);
    if (dialog.exec() == QDialog::Accepted) {
        int fromId = dialog.getFromId();
        int toId = dialog.getToId();

        if (fromId < 0 || toId < 0) {
            QMessageBox::warning(this, "Error", "ID needed");
            return;
        }
        Vertex* v_from = m_graph->getVertexById(fromId);
        Vertex* v_to = m_graph->getVertexById(toId);

        if (!v_from || !v_to) {
            QMessageBox::warning(this, "Error", "Invalid ID");
            return;
        }

        if (fromId == toId) {
             QMessageBox::warning(this, "Error", "Loops are not allowed");
             return;
        }

        bool exists = m_graph->getEdge(fromId, toId) != nullptr;
        if (!m_graph->isDirected()) {
             exists = exists || (m_graph->getEdge(toId, fromId) != nullptr);
        }
        if (exists) {
             QMessageBox::warning(this, "Error", "Such edge already exists");
             return;
        }

        double weight = m_graph->isWeighted() ? dialog.getWeight() : 1.0;
        Edge* newEdge = new Edge(v_from, v_to, weight);

        m_graph->addEdge(newEdge);
        m_scene->addEdge(newEdge);
        updateGraphUI();
        resetAlgorithm();
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
        QMessageBox::information(this, "Remove Vertex", "Select a vertex to remove");
        return;
    }

    Vertex* vertexToRemove = selectedItem->getVertex();
    if (!vertexToRemove) return;
    int idToRemove = vertexToRemove->getId();

    for (Edge* edge : m_graph->getEdges()) {
        if (edge && edge->isActive() &&
           (edge->getFrom() == idToRemove || edge->getTo() == idToRemove))
        {
            m_scene->removeEdge(edge);
        }
    }

    m_graph->removeVertex(idToRemove);
    m_scene->removeVertex(vertexToRemove);

    updateGraphUI();
    resetAlgorithm();
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
        QMessageBox::information(this, "Remove edge", "Select an edge to remove");
        return;
    }
    Edge* edgeToRemove = selectedItem->getEdge();
    if (!edgeToRemove) return;

    m_scene->removeEdge(edgeToRemove);
    m_graph->removeEdge(edgeToRemove);

    updateGraphUI();
    resetAlgorithm();
}

void MainWindow::updateGraphUI() {
    if (!m_graph) return;

    m_controlPanel->updateStartVertexComboBox(m_graph->getVertices());
    m_scene->updateEdgeWeights();

    bool hasActiveVertices = false;
    for (auto v : m_graph->getVertices()) {
        if (v && v->isActive()) {
            hasActiveVertices = true;
            break;
        }
    }
    m_controlPanel->setAlgorithmControlsEnabled(hasActiveVertices);
}

void MainWindow::onAlgorithmChanged(AlgorithmType type) {
    if (type == AlgorithmType::Dijkstra && m_graph && !m_graph->isWeighted()) {
        QMessageBox::warning(this, "Warning", "Dijkstra algorithm is best used on a weighted graph.");
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
        QMessageBox::warning(this, "Error", "The graph is empty or the starting vertex is not initialised");
        resetAlgorithm();
    }
}

void MainWindow::nextStep() {
    if (m_algoController->nextStep(m_currentState)) {
        applyState(m_currentState);
        m_controlPanel->getPrevButton()->setEnabled(true);
    } else {
        if (m_playTimer->isActive()) {
            onPauseClicked();
        }
        applyState(m_currentState);

        m_controlPanel->getNextButton()->setEnabled(false);
        m_controlPanel->getPlayButton()->setEnabled(false);

        if (!m_currentState.message.empty()) {
             QMessageBox::information(this, "Done", QString::fromStdString(m_currentState.message));
        } else {
             QMessageBox::information(this, "Done", "The end");
        }
    }
}

void MainWindow::prevStep() {
    if (m_playTimer->isActive()) {
        onPauseClicked();
    }

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

    if (m_graph) {
        m_controlPanel->updateStartVertexComboBox(m_graph->getVertices());

        bool hasActiveVertices = false;
        for (auto v : m_graph->getVertices()) {
            if (v && v->isActive()) {
                hasActiveVertices = true;
                break;
            }
        }
        m_controlPanel->setAlgorithmControlsEnabled(hasActiveVertices);
    }
}

void MainWindow::onPlayClicked() {
    if (m_algoController->getCurrentStep() == 0 && m_controlPanel->getStartButton()->isEnabled()) {
         m_controlPanel->getStartButton()->click();
         if(m_algoController->getCurrentStep() == 0) {
             QMessageBox::warning(this, "Error", "Press 'Start' or select a valid start vertex.");
             return;
         }
    }
    m_playTimer->start(m_playSpeedMs);
    m_controlPanel->getPlayButton()->setEnabled(false);
    m_controlPanel->getPauseButton()->setEnabled(true);
}

void MainWindow::onPauseClicked() {
    bool wasActive = m_playTimer->isActive();
    m_playTimer->stop();

    if(wasActive) {
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
    const QColor currentEdgeColor(Qt::red);
    const QColor pathColor(Qt::magenta);

    bool isDijkstra = (m_algoController->getAlgorithm() == AlgorithmType::Dijkstra);

    if (isDijkstra && !state.distances.empty()) {
        for (Vertex* v : m_graph->getVertices()) {
            if (v && v->isActive()) {
                VertexItem* vItem = m_scene->getVertexItem(v);
                if (vItem) {
                    if (v->getId() >= static_cast<int>(state.distances.size())) continue;
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
        Edge* edge = m_graph->getEdge(id.from, id.to);
        if (!edge && !m_graph->isDirected()) edge = m_graph->getEdge(id.to, id.from);

        EdgeItem* eItem = m_scene->getEdgeItem(edge);
        if (eItem) eItem->setColor(treeEdgeColor);
    }

    for (int vertexId : state.visitedVertices) {
        Vertex* v = m_graph->getVertexById(vertexId);
        VertexItem* vItem = m_scene->getVertexItem(v);
        if (vItem) vItem->setColor(visitedColor);
    }

    for (int vertexId : state.frontier) {
         Vertex* v = m_graph->getVertexById(vertexId);
        VertexItem* vItem = m_scene->getVertexItem(v);
        if (vItem && vItem->brush().color() != visitedColor) {
            vItem->setColor(frontierColor);
        }
    }

    for (const EdgeId& id : state.currentEdges) {
        Edge* edge = m_graph->getEdge(id.from, id.to);
        if (!edge && !m_graph->isDirected()) edge = m_graph->getEdge(id.to, id.from);

        EdgeItem* eItem = m_scene->getEdgeItem(edge);
        if (eItem) eItem->setColor(currentEdgeColor);
    }

    if (state.currentVertex >= 0) {
        Vertex* v = m_graph->getVertexById(state.currentVertex);
        VertexItem* vItem = m_scene->getVertexItem(v);
        if (vItem) vItem->setColor(processingColor);
    }

    if (!state.shortestPathEdges.empty()) {
        std::set<int> pathVertices;

        for (const EdgeId& id : state.shortestPathEdges) {
            pathVertices.insert(id.from);
            pathVertices.insert(id.to);

            Edge* edge = m_graph->getEdge(id.from, id.to);
            if (!edge && !m_graph->isDirected()) edge = m_graph->getEdge(id.to, id.from);

            EdgeItem* eItem = m_scene->getEdgeItem(edge);
            if (eItem) {
                eItem->setColor(pathColor);
                eItem->setZValue(2);
            }
        }

        for (int vertexId : pathVertices) {
             Vertex* v = m_graph->getVertexById(vertexId);
             VertexItem* vItem = m_scene->getVertexItem(v);
             if (vItem) {
                vItem->setColor(pathColor);
                vItem->setZValue(3);
             }
        }
    }
}

void MainWindow::resetAlgorithmStyles() {
    m_scene->resetAlgorithmStyles();
}

void MainWindow::onApplyLayout() {
    if (!m_graph) return;
    onPauseClicked();
    resetAlgorithm();
    GraphvizEngine::LayoutAlgorithm algo = GraphvizEngine::getDefaultLayoutAlgorithm(m_graph->isDirected());

    QMap<int, QPointF> positions = m_graphvizEngine->layoutGraph(m_graph.get(), algo);
    m_scene->applyLayout(positions);

    m_graphicsView->centerOn(m_scene->sceneRect().center());
}

void MainWindow::onSetExportDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Export Directory"),
        m_currentExportDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_currentExportDir = dir;
    }
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
    else if (formatId == "jpeg") {
         defaultExt = ".jpeg";
         formatId = "jpeg";
    } else {
        return;
    }

    QDir outputDir(m_currentExportDir);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::warning(this, "Error", "Could not create directory: " + m_currentExportDir);
            return;
        }
    }

    QString baseName = "graph";
    QString finalName = baseName + defaultExt;
    int counter = 1;

    while (outputDir.exists(finalName)) {
        finalName = QString("%1 (%2)%3").arg(baseName).arg(counter).arg(defaultExt);
        counter++;
    }

    QString filePath = outputDir.absoluteFilePath(finalName);

    GraphvizEngine::LayoutAlgorithm algo = GraphvizEngine::getDefaultLayoutAlgorithm(m_graph->isDirected());

    if (m_graphvizEngine->exportToFile(m_graph.get(), filePath, formatId, algo)) {
        QMessageBox::information(this, "Export Successful", "Saved to:\n" + filePath);
    } else {
        QMessageBox::warning(this, "Export Failed", "Could not save file to:\n" + filePath);
    }
}

void MainWindow::exportToDot() { onExportFromPanel("DOT"); }
void MainWindow::exportToPng() { onExportFromPanel("PNG"); }
void MainWindow::exportToSvg() { onExportFromPanel("SVG"); }
void MainWindow::exportToPdf() { onExportFromPanel("PDF"); }
void MainWindow::exportToJson() { onExportFromPanel("JSON"); }
void MainWindow::exportToJpeg() { onExportFromPanel("JPEG"); }