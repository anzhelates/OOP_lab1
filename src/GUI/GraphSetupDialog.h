#pragma once
#include <QDialog>
#include <vector>
#include <tuple>

struct GraphSettings {
    bool directed;
    bool weighted;
};

QT_BEGIN_NAMESPACE
namespace Ui { class GraphSetupDialog; }
QT_END_NAMESPACE

class GraphSetupDialog : public QDialog {
    Q_OBJECT
public:
    explicit GraphSetupDialog(QWidget* parent = nullptr);
    ~GraphSetupDialog() override;

    GraphSettings getSettings() const;
    int getVertexCount() const;
    std::vector<std::tuple<int, int, double>> getEdges() const;

private:
    Ui::GraphSetupDialog* ui;
};