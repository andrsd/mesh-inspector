// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clipwidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QStackedWidget>
#include <QVector3D>
#include <QButtonGroup>
#include "common/clickablelabel.h"
#include "common/doubleslider.h"

ClipWidget::ClipWidget(QWidget * parent) : QWidget(parent)
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
    this->layout->setContentsMargins(15, 8, 15, 6);

    this->plane_label = new QLabel("Clipping plane");
    this->layout->addWidget(this->plane_label);

    this->x_plane = new QRadioButton("YZ");
    this->y_plane = new QRadioButton("XZ");
    this->z_plane = new QRadioButton("XY");

    this->plane_group = new QButtonGroup();
    this->plane_group->addButton(this->x_plane, 0);
    this->plane_group->addButton(this->y_plane, 1);
    this->plane_group->addButton(this->z_plane, 2);

    this->layout->addWidget(this->x_plane);
    this->layout->addWidget(this->y_plane);
    this->layout->addWidget(this->z_plane);

    this->flip_plane_normal = new QCheckBox("Flip normal");
    this->layout->addWidget(this->flip_plane_normal);

    this->sliders_stack = new QStackedWidget();
    this->sliders_stack->setFixedWidth(200);
    for (int i = 0; i < 3; i++) {
        this->slider[i] = new DoubleSlider(Qt::Horizontal);
        this->sliders_stack->addWidget(this->slider[i]);
    }
    this->layout->addWidget(this->sliders_stack);

    this->layout->addSpacing(20);

    this->close = new ClickableLabel();
    this->close->setText("Close");
    this->close->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->close);

    this->setLayout(this->layout);

    connect(this->close, &ClickableLabel::clicked, this, &ClipWidget::onClose);
    connect(this->plane_group, &QButtonGroup::idClicked, this, &ClipWidget::onPlaneIdClicked);
    connect(this->flip_plane_normal, &QCheckBox::clicked, this, &ClipWidget::onFlipPlaneNormal);
    connect(this->slider[0], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
    connect(this->slider[1], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
    connect(this->slider[2], &QSlider::sliderMoved, this, &ClipWidget::onPlaneMoved);
}

void
ClipWidget::setClipPlane(int id)
{
    this->plane_group->button(id)->click();
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
ClipWidget::onPlaneIdClicked(int id)
{
    this->sliders_stack->setCurrentIndex(id);
    emit planeChanged(id);
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
