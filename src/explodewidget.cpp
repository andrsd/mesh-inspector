#include "explodewidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include "common/clickablelabel.h"

ExplodeWidget::ExplodeWidget(QWidget * parent) : QWidget(parent), max_range(100)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border-radius: 3px;"
                  "background-color: #222;"
                  "color: #fff;"
                  "font-size: 14px;");
    this->opacity = new QGraphicsOpacityEffect(this);
    this->opacity->setOpacity(0.8);
    setGraphicsEffect(this->opacity);

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 8);

    this->mag_validator = new QDoubleValidator();
    this->mag_validator->setBottom(0.);

    this->magnitude = new QLineEdit("1.0");
    this->magnitude->setValidator(this->mag_validator);
    this->magnitude->setFixedWidth(30);
    this->layout->addWidget(this->magnitude);

    this->x_lbl = new QLabel("x");
    this->layout->addWidget(this->x_lbl);

    this->slider = new QSlider(Qt::Horizontal, this);
    this->slider->setRange(0, this->max_range);
    this->slider->setSingleStep(1);
    this->slider->setPageStep(5);
    this->slider->setFixedWidth(400);
    this->layout->addWidget(this->slider);

    this->close = new ClickableLabel();
    this->close->setText("Close");
    this->close->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->close);

    this->setLayout(this->layout);

    connect(this->slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(this->magnitude, SIGNAL(editingFinished()), this, SLOT(onMagnitudeChanged()));
    connect(this->close, SIGNAL(clicked()), this, SLOT(onClose()));
}

ExplodeWidget::~ExplodeWidget() {}

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
ExplodeWidget::onClose()
{
    hide();
    emit closed();
}
