// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clipwidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include "common/clickablelabel.h"

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

    this->layout->addSpacing(20);

    this->close = new ClickableLabel();
    this->close->setText("Close");
    this->close->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->close);

    this->setLayout(this->layout);

    connect(this->close, &ClickableLabel::clicked, this, &ClipWidget::onClose);
    connect(this->plane_group, &QButtonGroup::idClicked, this, &ClipWidget::onPlaneIdClicked);
    connect(this->flip_plane_normal, &QCheckBox::clicked, this, &ClipWidget::onFlipPlaneNormal);
}

void
ClipWidget::setClipPlane(int id)
{
    this->plane_group->button(id)->click();
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
    emit planeChanged(id);
}

void
ClipWidget::onFlipPlaneNormal()
{
    emit planeNormalFlipped();
}
