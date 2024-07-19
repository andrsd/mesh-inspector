// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cliptool.h"
#include "clipwidget.h"
#include "mainwindow.h"
#include "model.h"
#include "blockobject.h"
#include "vtkPlane.h"
#include "vtkClipPolyData.h"
#include "vtkPolyDataPlaneClipper.h"
#include "vtkVector.h"

ClipTool::ClipTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    widget(nullptr),
    clip_plane(vtkPlane::New()),
    normal(0, 0, 1),
    normal_ori(1.)
{
}

ClipTool::~ClipTool()
{
    delete this->widget;
    this->clip_plane->Delete();
}

void
ClipTool::setPlaneNormal(const QVector3D & n)
{
    this->normal = n;
    QVector3D tmp = this->normal_ori * n;
    this->clip_plane->SetNormal(tmp.x(), tmp.y(), tmp.z());
    updateModelBlocks();
}

void
ClipTool::setPlaneOrigin(const QVector3D & pt)
{
    this->clip_plane->SetOrigin(pt.x(), pt.y(), pt.z());
}

void
ClipTool::setupWidgets()
{
    this->widget = new ClipWidget(this->main_window);
    this->widget->setFixedHeight(40);
    this->widget->setVisible(false);

    QMainWindow::connect(this->widget, &ClipWidget::closed, this, &ClipTool::onClose);
    QMainWindow::connect(this->widget, &ClipWidget::planeChanged, this, &ClipTool::onPlaneChanged);
    QMainWindow::connect(this->widget,
                         &ClipWidget::planeNormalFlipped,
                         this,
                         &ClipTool::onPlaneNormalFlipped);
    QMainWindow::connect(this->widget, &ClipWidget::planeMoved, this, &ClipTool::onPlaneMoved);

    this->widget->setClipPlane(2);
}

void
ClipTool::updateLocation()
{
    auto width = this->main_window->getRenderWindowWidth();
    int left = (width - this->widget->width()) / 2;
    int top = this->main_window->geometry().height() - this->widget->height() - 10;
    this->widget->move(left, top);
}

void
ClipTool::onClip()
{
    auto bbox = this->model->getTotalBoundingBox();
    auto max_pt = bbox.GetMaxPoint();
    auto min_pt = bbox.GetMinPoint();
    QVector3D qmin_pt(min_pt[0], min_pt[1], min_pt[2]);
    QVector3D qmax_pt(max_pt[0], max_pt[1], max_pt[2]);
    this->widget->setBoundingBox(qmin_pt, qmax_pt);

    auto cob = this->model->getCenterOfBounds();
    QVector3D ctr(cob[0], cob[1], cob[2]);
    this->widget->setOrigin(ctr);
    this->clip_plane->SetOrigin(cob.GetData());

    this->widget->adjustSize();
    this->widget->show();
    updateLocation();

    clipBlocks();
}

void
ClipTool::onClose()
{
    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;
        block->setClip(false);
    }
}

void
ClipTool::clipBlocks()
{
    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;
        block->setClip(true);
        block->setClipPlane(this->clip_plane);
    }
}

void
ClipTool::onPlaneChanged(int id)
{
    if (id == 0)
        setPlaneNormal(QVector3D(1, 0, 0));
    else if (id == 1)
        setPlaneNormal(QVector3D(0, 1, 0));
    else if (id == 2)
        setPlaneNormal(QVector3D(0, 0, 1));
}

void
ClipTool::onPlaneNormalFlipped()
{
    this->normal_ori *= -1;
    setPlaneNormal(this->normal);
    updateModelBlocks();
}

void
ClipTool::onPlaneMoved()
{
    auto origin = this->widget->origin();
    this->clip_plane->SetOrigin(origin.x(), origin.y(), origin.z());
    updateModelBlocks();
}

void
ClipTool::updateModelBlocks()
{
    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;
        block->setClipPlane(this->clip_plane);
        block->modified();
        block->update();
    }
}
