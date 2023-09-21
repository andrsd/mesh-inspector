#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class QDoubleValidator;
class QLineEdit;
class QLabel;
class QSlider;
class ClickableLabel;

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
    void onClose();

protected:
    int max_range;
    QGraphicsOpacityEffect * opacity;
    QHBoxLayout * layout;
    QDoubleValidator * mag_validator;
    QLineEdit * magnitude;
    QLabel * x_lbl;
    QSlider * slider;
    ClickableLabel * close;
};
