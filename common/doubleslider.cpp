// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "doubleslider.h"
#include <iostream>

int DoubleSlider::UPPER = 10000;

DoubleSlider::DoubleSlider(QWidget * parent) : QSlider(parent) {}

DoubleSlider::DoubleSlider(Qt::Orientation orientation, QWidget * parent) :
    QSlider(orientation, parent)
{
    connect(this, &QSlider::sliderMoved, this, &DoubleSlider::onSliderMoved);
}

void
DoubleSlider::setRange(double low, double high)
{
    if (low < high) {
        this->lower = low;
        this->upper = high;
    }
    else {
        this->lower = high;
        this->upper = low;
    }
    QSlider::setRange(0, UPPER);
}

double
DoubleSlider::value() const
{
    return this->val;
}

void
DoubleSlider::setValue(double value)
{
    this->val = value;
    double i = (value - this->lower) / (this->upper - this->lower);
    QSlider::setValue(i * UPPER);
}

void
DoubleSlider::onSliderMoved(int value)
{
    this->val = (static_cast<double>(value) / UPPER) * (this->upper - this->lower) + this->lower;
    emit sliderMoved(this->val);
}
