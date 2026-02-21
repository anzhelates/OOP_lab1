#include "AddEdgeDialog.h"
#include "ui_AddEdgeDialog.h"

AddEdgeDialog::AddEdgeDialog(bool weighted, int maxVertexId, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddEdgeDialog)
{
    ui->setupUi(this);

    ui->weightLabel->setVisible(weighted);
    ui->weightSpinBox->setVisible(weighted);

    if (maxVertexId < 0) maxVertexId = 0;

    ui->fromSpinBox->setMinimum(0);
    ui->fromSpinBox->setMaximum(maxVertexId);
    ui->fromSpinBox->setValue(0);

    ui->toSpinBox->setMinimum(0);
    ui->toSpinBox->setMaximum(maxVertexId);
    ui->toSpinBox->setValue(0);

    ui->weightSpinBox->setMinimum(0.01);
    ui->weightSpinBox->setValue(1.0);

    if (!weighted) {
        ui->weightSpinBox->setValue(1.0);
    }
}

AddEdgeDialog::~AddEdgeDialog() {
    delete ui;
}

int AddEdgeDialog::getFromId() const {
    return ui->fromSpinBox->value();
}

int AddEdgeDialog::getToId() const {
    return ui->toSpinBox->value();
}

double AddEdgeDialog::getWeight() const {
    return ui->weightSpinBox->value();
}