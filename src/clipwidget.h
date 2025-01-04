// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class QFormLayout;
class QLabel;
class QComboBox;
class QCheckBox;
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
    void onPlaneIndexChanged(int index);
    void onFlipPlaneNormal();
    void onPlaneMoved(double value);

protected:
    void closeEvent(QCloseEvent * event) override;

protected:
    QFormLayout * layout;
    QComboBox * plane;
    QCheckBox * flip_plane_normal;
    QStackedWidget * sliders_stack;
    std::array<DoubleSlider *, 3> slider;
};
