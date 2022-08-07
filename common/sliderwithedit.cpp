#include "sliderwithedit.h"
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>

SliderWithEdit::SliderWithEdit(QWidget * parent) :
    QWidget(parent),
    layout(nullptr),
    slider(nullptr),
    spin(nullptr),
    min(0),
    max(10),
    width(45)
{
    this->slider = new QSlider(Qt::Horizontal);
    this->slider->setRange(this->min, this->max);
    this->slider->setSingleStep(1);
    this->slider->setPageStep(10);

    this->spin = new QSpinBox(parent);
    this->spin->setFixedWidth(this->width);
    this->spin->setRange(this->min, this->max);
    this->spin->setSingleStep(1);

    this->layout = new QHBoxLayout(parent);
    this->layout->addWidget(this->slider);
    this->layout->addWidget(this->spin);
    setLayout(this->layout);

    connect(this->slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
    connect(this->spin, SIGNAL(valueChanged(int)), this, SLOT(onSpinValueChanged(int)));
}

SliderWithEdit::~SliderWithEdit()
{
    delete this->slider;
    delete this->spin;
    delete this->layout;
}

void
SliderWithEdit::setRange(int min, int max)
{
    this->slider->setRange(min, max);
    this->spin->setRange(min, max);
    this->min = min;
    this->max = max;
}

void
SliderWithEdit::onSliderChanged(int value)
{
    this->blockSignals(true);
    this->spin->setValue(value);
    this->blockSignals(false);
}

void
SliderWithEdit::onSpinValueChanged(int value)
{
    this->blockSignals(true);
    this->slider->setValue(value);
    this->blockSignals(false);
}
