#pragma once
#include <QWidget>
#include <vector>
#include "Vertex.h"
#include "GraphvizEngine.h"

enum class AlgorithmType;

QT_BEGIN_NAMESPACE
class QPushButton;
namespace Ui { class ControlPanel; }
QT_END_NAMESPACE

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel();

    void setGraphEditingEnabled(bool enabled);
    void setAlgorithmControlsEnabled(bool enabled);
    void setExportEnabled(bool enabled);
    void updateStartVertexComboBox(const std::vector<Vertex*>& vertices);
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
        void algorithmChanged(AlgorithmType type);
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
        void setupGraphClicked();
        void onAddVertexClicked();
        void onAddEdgeClicked();
        void onRemoveVertexClicked();
        void onRemoveEdgeClicked();
        void onApplyLayoutClicked();
        void onExportClicked();
        void onChooseDirClicked();

private:
    Ui::ControlPanel* ui;
};