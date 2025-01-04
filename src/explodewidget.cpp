// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "explodewidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>

ExplodeWidget::ExplodeWidget(QWidget * parent) : QWidget(parent), max_range(100)
{
    setWindowTitle("Explode");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(40);
    setFixedWidth(450);

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 6);

    this->mag_validator = new QDoubleValidator();
    this->mag_validator->setBottom(0.);

    this->magnitude = new QLineEdit("1.0");
    this->magnitude->setValidator(this->mag_validator);
    this->magnitude->setFixedWidth(30);
    this->layout->addWidget(this->magnitude);

    this->x_lbl = new QLabel("x");
    this->x_lbl->setFixedWidth(8);
    this->layout->addWidget(this->x_lbl);

    this->slider = new QSlider(Qt::Horizontal, this);
    this->slider->setRange(0, this->max_range);
    this->slider->setSingleStep(1);
    this->slider->setPageStep(5);
    this->layout->addWidget(this->slider);

    this->setLayout(this->layout);

    connect(this->slider, &QSlider::valueChanged, this, &ExplodeWidget::onSliderValueChanged);
    connect(this->magnitude, &QLineEdit::editingFinished, this, &ExplodeWidget::onMagnitudeChanged);
}

int
ExplodeWidget::range()
{
    return this->max_range;
}

void
ExplodeWidget::onSliderValueChanged(int value)
{
    double scale = this->magnitude->text().toDouble();
    emit valueChanged(scale * value);
}

void
ExplodeWidget::onMagnitudeChanged()
{
    double scale = this->magnitude->text().toDouble();
    auto value = this->slider->value();
    emit valueChanged(scale * value);
}

void
ExplodeWidget::closeEvent(QCloseEvent * event)
{
    emit closed();
    QWidget::closeEvent(event);
}
