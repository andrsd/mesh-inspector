// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meshqualitywidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtDebug>

MeshQualityWidget::MeshQualityWidget(QWidget * parent) : QWidget(parent)
{
    setWindowTitle("Mesh quality");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(40);
    setFixedWidth(250);

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 8);

    this->metric_label = new QLabel();
    this->metric_label->setText("Metric");
    this->metric_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->metric_label->setFixedWidth(50);
    this->layout->addWidget(this->metric_label);

    this->metric = new QComboBox();
    this->metric->setEditable(false);
    this->metric->addItem("Jacobian", MESH_METRIC_JACOBIAN);
    this->metric->addItem("Aspect ratio", MESH_METRIC_ASPECT_RATIO);
    this->metric->addItem("Condition number", MESH_METRIC_CONDITION);
    this->metric->addItem("Area", MESH_METRIC_AREA);
    this->metric->addItem("Volume", MESH_METRIC_VOLUME);
    this->layout->addWidget(this->metric);

    this->setLayout(this->layout);

    connect(this->metric,
            &QComboBox::currentIndexChanged,
            this,
            &MeshQualityWidget::onMetricChanged);

    this->metric->setCurrentIndex(0);
}

void
MeshQualityWidget::onMetricChanged(int index)
{
    auto id = getMetricId();
    emit metricChanged(id);
}

int
MeshQualityWidget::getMetricId()
{
    return this->metric->currentData().toInt();
}

void
MeshQualityWidget::done()
{
    if (isVisible()) {
        hide();
        emit closed();
    }
}

void
MeshQualityWidget::closeEvent(QCloseEvent * event)
{
    emit closed();
    QWidget::closeEvent(event);
}
