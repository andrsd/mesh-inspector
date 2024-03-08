// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common_export.h"
#include <QWidget>

class QHBoxLayout;
class QSlider;
class QSpinBox;

class SliderWithEdit : public QWidget {
public:
    explicit SliderWithEdit(QWidget * parent = nullptr);
    ~SliderWithEdit() override;

    void setRange(int min, int max);

protected slots:
    void onSliderChanged(int value);
    void onSpinValueChanged(int value);

protected:
    QHBoxLayout * layout;
    QSlider * slider;
    QSpinBox * spin;

    int min;
    int max;
    int width;
};
