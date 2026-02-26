#pragma once
#include <QWidget>
#include <vector>
#include <QString>
#include "AlgorithmController.h"
#include "Vertex.h"

QT_BEGIN_NAMESPACE
class QPushButton;
namespace Ui { class ControlPanel; }
QT_END_NAMESPACE

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel() override;

    void setGraphEditingEnabled(bool enabled);
    void setAlgorithmControlsEnabled(bool enabled);
    void setExportEnabled(bool enabled);
    void updateStartVertexComboBox(const std::vector<const Core::Vertex*>& vertices);
    void resetPlayerControls();

    QPushButton* getNextButton() const;
    QPushButton* getPrevButton() const;
    QPushButton* getPlayButton() const;
    QPushButton* getPauseButton() const;
    QPushButton* getStartButton() const;

    signals:
        void startAlgorithmClicked(int startId, int endId);
    void pauseAlgorithmClicked();
    void playAlgorithmClicked();
    void speedChanged(int speed);
    void resetClicked();
    void nextClicked();
    void prevClicked();
    void algorithmChanged(Algorithms::AlgorithmType type);
    void setupGraphButtonClicked();
    void addVertexClicked();
    void addEdgeClicked();
    void removeVertexClicked();
    void removeEdgeClicked();
    void applyLayoutClicked();
    void exportClicked(const QString& format);
    void chooseDirClicked();

    private slots:
        void onStartAlgorithmClicked();
    void onPauseAlgorithmClicked();
    void onPlayAlgorithmClicked();
    void onSpeedSliderChanged(int value);
    void onResetClicked();
    void onAlgorithmChanged(int index);
    void onExportClicked();

private:
    Ui::ControlPanel* ui;
};