#include "GraphSetupDialog.h"
#include "ui_GraphSetupDialog.h"
#include <QStringList>
#include <QRegularExpression>

GraphSetupDialog::GraphSetupDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::GraphSetupDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

GraphSetupDialog::~GraphSetupDialog() {
    delete ui;
}

GraphSettings GraphSetupDialog::getSettings() const {
    return {
        ui->directedCheckBox->isChecked(),
        ui->weightedCheckBox->isChecked()
    };
}

int GraphSetupDialog::getVertexCount() const {
    return ui->vertexCountSpinBox->value();
}

std::vector<std::tuple<int, int, double>> GraphSetupDialog::getEdges() const {
    std::vector<std::tuple<int, int, double>> edges;
    QString text = ui->edgesTextEdit->toPlainText();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QStringList parts = line.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool isSrc, isDst;
            int src = parts[0].toInt(&isSrc);
            int dst = parts[1].toInt(&isDst);
            double weight = 1.0;

            if (parts.size() >= 3) {
                bool isWeight;
                weight = parts[2].toDouble(&isWeight);
                if (!isWeight) weight = 1.0;
            }

            if (isSrc && isDst) {
                edges.emplace_back(src, dst, weight);
            }
        }
    }
    return edges;
}