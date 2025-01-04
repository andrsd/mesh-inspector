// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clipwidget.h"
#include <QGraphicsOpacityEffect>
#include <QFormLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QStackedWidget>
#include <QVector3D>
#include <QButtonGroup>
#include <QComboBox>
#include "common/doubleslider.h"

ClipWidget::ClipWidget(QWidget * parent) : QWidget(parent)
{
    setWindowTitle("Clip");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(100);
    setFixedWidth(200);

    this->layout = new QFormLayout();
    this->layout->setContentsMargins(15, 8, 15, 6);
    this->layout->setSpacing(6);
    this->layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    this->plane = new QComboBox();
    this->plane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->plane->addItem("YZ", 0);
    this->plane->addItem("XZ", 1);
    this->plane->addItem("XY", 2);
    this->layout->addRow("Plane", this->plane);

    this->flip_plane_normal = new QCheckBox("");
    this->layout->addRow("Flip normal", this->flip_plane_normal);

    this->sliders_stack = new QStackedWidget();
    this->sliders_stack->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 3; i++) {
        this->slider[i] = new DoubleSlider(Qt::Horizontal);
        this->sliders_stack->addWidget(this->slider[i]);
    }
    this->layout->addRow(this->sliders_stack);

    this->setLayout(this->layout);

    connect(this->plane, &QComboBox::currentIndexChanged, this, &ClipWidget::onPlaneIndexChanged);
    connect(this->flip_plane_normal, &QCheckBox::clicked, this, &ClipWidget::onFlipPlaneNormal);
    connect(this->slider[0], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
    connect(this->slider[1], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
    connect(this->slider[2], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
}

void
ClipWidget::setClipPlane(int id)
{
    this->plane->setCurrentIndex(id);
}

void
ClipWidget::setBoundingBox(const QVector3D & min_pt, const QVector3D & max_pt)
{
    for (int i = 0; i < 3; i++)
        this->slider[i]->setRange(min_pt[i], max_pt[i]);
}

void
ClipWidget::setOrigin(const QVector3D & origin)
{
    for (int i = 0; i < 3; i++)
        this->slider[i]->setValue(origin[i]);
}

QVector3D
ClipWidget::origin() const
{
    return QVector3D(this->slider[0]->value(), this->slider[1]->value(), this->slider[2]->value());
}

void
ClipWidget::onClose()
{
    hide();
    emit closed();
}

void
ClipWidget::onPlaneIndexChanged(int index)
{
    this->sliders_stack->setCurrentIndex(index);
    emit planeChanged(index);
}

void
ClipWidget::onFlipPlaneNormal()
{
    emit planeNormalFlipped();
}

void
ClipWidget::onPlaneMoved(double value)
{
    emit planeMoved();
}

void
ClipWidget::done()
{
    if (isVisible())
        onClose();
}

void
ClipWidget::closeEvent(QCloseEvent * event)
{
    emit closed();
    QWidget::closeEvent(event);
}
