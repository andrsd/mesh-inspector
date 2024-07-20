// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class QLabel;
class ClickableLabel;
class QButtonGroup;
class QCheckBox;
class QRadioButton;
class QStackedWidget;
class DoubleSlider;
class QVector3D;

class ClipWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipWidget(QWidget * parent = nullptr);

    void setClipPlane(int id);
    void setBoundingBox(const QVector3D & min_pt, const QVector3D & max_pt);
    void setOrigin(const QVector3D & origin);
    QVector3D origin() const;
    void done();

signals:
    void closed();
    void planeChanged(int id);
    void planeNormalFlipped();
    void planeMoved();

protected slots:
    void onClose();
    void onPlaneIdClicked(int id);
    void onFlipPlaneNormal();
    void onPlaneMoved(double value);

protected:
    QGraphicsOpacityEffect * opacity;
    QHBoxLayout * layout;
    QLabel * plane_label;
    QButtonGroup * plane_group;
    QRadioButton * x_plane;
    QRadioButton * y_plane;
    QRadioButton * z_plane;
    QCheckBox * flip_plane_normal;
    QStackedWidget * sliders_stack;
    std::array<DoubleSlider *, 3> slider;
    ClickableLabel * close;
};
