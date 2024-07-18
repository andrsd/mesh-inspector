// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QCheckBox>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class ClickableLabel;

class ClipWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipWidget(QWidget * parent = nullptr);

    void setClipPlane(int id);

signals:
    void closed();
    void planeChanged(int id);
    void planeNormalFlipped();

protected slots:
    void onClose();
    void onPlaneIdClicked(int id);
    void onFlipPlaneNormal();

protected:
    QGraphicsOpacityEffect * opacity;
    QHBoxLayout * layout;
    QLabel * plane_label;
    QButtonGroup * plane_group;
    QRadioButton * x_plane;
    QRadioButton * y_plane;
    QRadioButton * z_plane;
    QCheckBox * flip_plane_normal;
    ClickableLabel * close;
};
