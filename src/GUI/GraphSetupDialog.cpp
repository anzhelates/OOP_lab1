#include "GraphSetupDialog.h"
#include "ui_GraphSetupDialog.h"
#include <QStringList>
#include <QRegularExpression>

GraphSetupDialog::GraphSetupDialog(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::GraphSetupDialog)
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

std::vector<std::tuple<int,int,int>> GraphSetupDialog::getEdges() const {
    std::vector<std::tuple<int,int,int>> edges;
    QString text = ui->edgesTextEdit->toPlainText();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QStringList parts = line.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool isScr, isDst;
            int src = parts[0].toInt(&isScr);
            int dst = parts[1].toInt(&isDst);
            int weight = 1;

            if (parts.size() >= 3) {
                bool isWeight;
                weight = parts[2].toInt(&isWeight);
                if (!isWeight) weight = 1;
            }

            if(isScr && isDst) {
                 edges.emplace_back(src, dst, weight);
            }
        }
    }
    return edges;
}