#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AddEdgeDialog; }
QT_END_NAMESPACE

class AddEdgeDialog : public QDialog {
    Q_OBJECT
    public:
    explicit AddEdgeDialog(bool weighted, int maxVertexId, QWidget* parent = nullptr);
    ~AddEdgeDialog();

    int getFromId() const;
    int getToId() const;
    double getWeight() const;

private:
    Ui::AddEdgeDialog* ui;
};