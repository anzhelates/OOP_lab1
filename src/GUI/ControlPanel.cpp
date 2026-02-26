#include "ControlPanel.h"
#include "ui_ControlPanel.h"
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QVariant>
#include <cmath>
#include <algorithm>

using namespace Algorithms;

ControlPanel::ControlPanel(QWidget* parent)
    : QWidget(parent), ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    ui->algorithmComboBox->addItem("BFS", QVariant::fromValue(AlgorithmType::BFS));
    ui->algorithmComboBox->addItem("DFS", QVariant::fromValue(AlgorithmType::DFS));
    ui->algorithmComboBox->addItem("Dijkstra", QVariant::fromValue(AlgorithmType::Dijkstra));

    QStringList exportFormats = {"PNG", "SVG", "DOT", "PDF", "JSON", "JPEG"};
    ui->exportFormatComboBox->addItems(exportFormats);

    ui->endVertexLabel->setVisible(false);
    ui->endVertexComboBox->setVisible(false);

    connect(ui->setupGraphButton, &QPushButton::clicked, this, &ControlPanel::setupGraphButtonClicked);
    connect(ui->addVertexButton, &QPushButton::clicked, this, &ControlPanel::addVertexClicked);
    connect(ui->addEdgeButton, &QPushButton::clicked, this, &ControlPanel::addEdgeClicked);
    connect(ui->removeVertexButton, &QPushButton::clicked, this, &ControlPanel::removeVertexClicked);
    connect(ui->removeEdgeButton, &QPushButton::clicked, this, &ControlPanel::removeEdgeClicked);
    connect(ui->applyLayoutButton, &QPushButton::clicked, this, &ControlPanel::applyLayoutClicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &ControlPanel::onExportClicked);
    connect(ui->chooseDirButton, &QPushButton::clicked, this, &ControlPanel::chooseDirClicked);

    connect(ui->startButton, &QPushButton::clicked, this, &ControlPanel::onStartAlgorithmClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &ControlPanel::onResetClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &ControlPanel::nextClicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &ControlPanel::prevClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &ControlPanel::onPlayAlgorithmClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &ControlPanel::onPauseAlgorithmClicked);
    connect(ui->speedSlider, &QSlider::valueChanged, this, &ControlPanel::onSpeedSliderChanged);
    connect(ui->algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ControlPanel::onAlgorithmChanged);

    setGraphEditingEnabled(false);
    setAlgorithmControlsEnabled(false);
    setExportEnabled(false);
    onSpeedSliderChanged(ui->speedSlider->value());
}

ControlPanel::~ControlPanel() {
    delete ui;
}

void ControlPanel::setGraphEditingEnabled(bool enabled) {
    ui->graphEditBox->setEnabled(enabled);
    ui->layoutBox->setEnabled(enabled);
}

void ControlPanel::setAlgorithmControlsEnabled(bool enabled) {
    ui->algorithmBox->setEnabled(enabled);
    ui->playerBox->setEnabled(enabled);
    if (enabled) resetPlayerControls();
}

void ControlPanel::setExportEnabled(bool enabled) {
    ui->exportBox->setEnabled(enabled);
}

void ControlPanel::updateStartVertexComboBox(const std::vector<const Core::Vertex*>& vertices) {
    int currentStartId = ui->startVertexComboBox->count() > 0 ? ui->startVertexComboBox->currentData().toInt() : -1;
    int currentEndId = ui->endVertexComboBox->count() > 0 ? ui->endVertexComboBox->currentData().toInt() : -1;

    ui->startVertexComboBox->clear();
    ui->endVertexComboBox->clear();

    bool hasActive = false;
    for (const auto* v : vertices) {
        if (v && v->isActive()) {
            QString name = QString::fromStdString(v->getName());
            ui->startVertexComboBox->addItem(name, QVariant(v->getId()));
            ui->endVertexComboBox->addItem(name, QVariant(v->getId()));
            hasActive = true;
        }
    }

    int newStartIdx = ui->startVertexComboBox->findData(currentStartId);
    ui->startVertexComboBox->setCurrentIndex(newStartIdx != -1 ? newStartIdx : 0);

    int newEndIdx = ui->endVertexComboBox->findData(currentEndId);
    ui->endVertexComboBox->setCurrentIndex(newEndIdx != -1 ? newEndIdx : std::max(0, ui->endVertexComboBox->count() - 1));

    ui->algorithmBox->setEnabled(hasActive);
    ui->playerBox->setEnabled(hasActive);
    ui->startButton->setEnabled(hasActive);
}

void ControlPanel::resetPlayerControls() {
    ui->startButton->setEnabled(ui->startVertexComboBox->count() > 0);
    ui->playButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->prevButton->setEnabled(false);
    ui->resetButton->setEnabled(false);

    ui->setupGraphButton->setEnabled(true);
    ui->graphEditBox->setEnabled(true);
    ui->layoutBox->setEnabled(true);
    ui->exportBox->setEnabled(true);
    ui->algorithmBox->setEnabled(true);
}

void ControlPanel::onStartAlgorithmClicked() {
    if (ui->startVertexComboBox->count() == 0) return;

    int startId = ui->startVertexComboBox->currentData().toInt();
    AlgorithmType type = ui->algorithmComboBox->currentData().value<AlgorithmType>();
    int endId = (type == AlgorithmType::Dijkstra) ? ui->endVertexComboBox->currentData().toInt() : -1;

    ui->setupGraphButton->setEnabled(false);
    ui->graphEditBox->setEnabled(false);
    ui->layoutBox->setEnabled(false);
    ui->exportBox->setEnabled(false);
    ui->algorithmBox->setEnabled(false);
    ui->startButton->setEnabled(false);

    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->nextButton->setEnabled(true);
    ui->prevButton->setEnabled(false);
    ui->resetButton->setEnabled(true);

    emit startAlgorithmClicked(startId, endId);
}

void ControlPanel::onPauseAlgorithmClicked() {
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->nextButton->setEnabled(true);
    ui->prevButton->setEnabled(true);
    emit pauseAlgorithmClicked();
}

void ControlPanel::onPlayAlgorithmClicked() {
    ui->playButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->nextButton->setEnabled(false);
    ui->prevButton->setEnabled(false);
    emit playAlgorithmClicked();
}

void ControlPanel::onResetClicked() {
    resetPlayerControls();
    emit resetClicked();
}

void ControlPanel::onAlgorithmChanged(int index) {
    AlgorithmType type = ui->algorithmComboBox->itemData(index).value<AlgorithmType>();
    bool isDijkstra = (type == AlgorithmType::Dijkstra);

    ui->endVertexLabel->setVisible(isDijkstra);
    ui->endVertexComboBox->setVisible(isDijkstra);

    emit algorithmChanged(type);
}

void ControlPanel::onSpeedSliderChanged(int value) {
    double minp = 0, maxp = 100;
    double minv = std::log(50), maxv = std::log(2000);
    double scale = (maxv - minv) / (maxp - minp);
    double speed = std::exp(minv + scale * (maxp - value));
    emit speedChanged(static_cast<int>(speed));
}

void ControlPanel::onExportClicked() {
    emit exportClicked(ui->exportFormatComboBox->currentText());
}

QPushButton* ControlPanel::getNextButton() const { return ui->nextButton; }
QPushButton* ControlPanel::getPrevButton() const { return ui->prevButton; }
QPushButton* ControlPanel::getPlayButton() const { return ui->playButton; }
QPushButton* ControlPanel::getPauseButton() const { return ui->pauseButton; }
QPushButton* ControlPanel::getStartButton() const { return ui->startButton; }