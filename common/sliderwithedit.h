#pragma once

#include <QWidget>

class QHBoxLayout;
class QSlider;
class QSpinBox;

class SliderWithEdit : public QWidget {
public:
    SliderWithEdit(QWidget * parent = nullptr);
    virtual ~SliderWithEdit();

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
