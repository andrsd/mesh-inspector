// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSlider>

class DoubleSlider : public QSlider {
    Q_OBJECT;

public:
    explicit DoubleSlider(QWidget * parent = nullptr);
    explicit DoubleSlider(Qt::Orientation orientation, QWidget * parent = nullptr);

    void setRange(double low, double high);
    double value() const;
    void setValue(double value);

signals:
    void sliderMoved(double value);

private slots:
    void onSliderMoved(int value);

private:
    double lower;
    double upper;
    double val;

    static int UPPER;
};
