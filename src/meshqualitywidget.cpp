// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meshqualitywidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtDebug>
#include "common/clickablelabel.h"

MeshQualityWidget::MeshQualityWidget(QWidget * parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("mesh-quality-wdgt");
    setStyleSheet("#mesh-quality-wdgt {"
                  "  border-radius: 3px;"
                  "  background-color: #222;"
                  "  color: #fff;"
                  "  font-size: 14px;"
                  "}"
                  "QLabel {"
                  "  background-color: #222;"
                  "  color: #fff;"
                  "}");
    this->opacity = new QGraphicsOpacityEffect(this);
    this->opacity->setOpacity(0.8);
    setGraphicsEffect(this->opacity);

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 8);

    this->metric_label = new QLabel();
    this->metric_label->setText("Metric");
    this->layout->addWidget(this->metric_label);

    this->metric = new QComboBox();
    this->metric->setEditable(false);
    this->metric->addItem("Jacobian", MESH_METRIC_JACOBIAN);
    this->metric->addItem("Aspect ratio", MESH_METRIC_ASPECT_RATIO);
    this->metric->addItem("Condition", MESH_METRIC_CONDITION);
    this->metric->addItem("Area", MESH_METRIC_AREA);
    this->metric->addItem("Volume", MESH_METRIC_VOLUME);
    this->layout->addWidget(this->metric);

    this->close = new ClickableLabel();
    this->close->setText("Close");
    this->close->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->close);

    this->setLayout(this->layout);

    connect(this->close, &ClickableLabel::clicked, this, &MeshQualityWidget::onClose);
    connect(this->metric,
            &QComboBox::currentIndexChanged,
            this,
            &MeshQualityWidget::onMetricChanged);

    this->metric->setCurrentIndex(0);
}

void
MeshQualityWidget::onClose()
{
    hide();
    emit closed();
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
    if (isVisible())
        onClose();
}
