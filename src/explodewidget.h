// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class QDoubleValidator;
class QLineEdit;
class QLabel;
class QSlider;

class ExplodeWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExplodeWidget(QWidget * parent = nullptr);

    int range();

signals:
    void closed();
    void valueChanged(double value);

protected slots:
    void onSliderValueChanged(int value);
    void onMagnitudeChanged();

protected:
    void closeEvent(QCloseEvent * event) override;

protected:
    int max_range;
    QHBoxLayout * layout;
    QDoubleValidator * mag_validator;
    QLineEdit * magnitude;
    QLabel * x_lbl;
    QSlider * slider;
};
