// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "clipwidget.h"
#include <QObject>
#include <QVector3D>
#include "vtkSmartPointer.h"
#include <QPoint>

class MainWindow;
class Model;
class vtkPlane;

class ClipTool : public QObject {
    Q_OBJECT

public:
    explicit ClipTool(MainWindow * main_wnd);
    ~ClipTool() override;

    void setupWidgets();
    void done();
    void setPlaneNormal(const QVector3D & n);
    void setPlaneOrigin(const QVector3D & pt);
    void closeEvent(QCloseEvent * event);

public slots:
    void onClip();

protected slots:
    void onClose();
    void onPlaneChanged(int id);
    void onPlaneNormalFlipped();
    void onPlaneMoved();

protected:
    void clipBlocks();
    void updateModelBlocks();

    MainWindow * main_window;
    Model *& model;
    ClipWidget * widget;
    vtkSmartPointer<vtkPlane> clip_plane;
    QVector3D normal;
    float normal_ori;
};
