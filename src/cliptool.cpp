// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cliptool.h"
#include "clipwidget.h"
#include "mainwindow.h"
#include "model.h"
#include "blockobject.h"
#include "vtkPlane.h"
#include "vtkVector.h"
#include <QSettings>

ClipTool::ClipTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    widget(nullptr),
    clip_plane(vtkSmartPointer<vtkPlane>::New()),
    normal(0, 0, 1),
    normal_ori(1.)
{
}

ClipTool::~ClipTool()
{
    delete this->widget;
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
    this->widget->setVisible(false);

    QMainWindow::connect(this->widget, &ClipWidget::closed, this, &ClipTool::onClose);
    QMainWindow::connect(this->widget, &ClipWidget::planeChanged, this, &ClipTool::onPlaneChanged);
    QMainWindow::connect(this->widget,
                         &ClipWidget::planeNormalFlipped,
                         this,
                         &ClipTool::onPlaneNormalFlipped);
    QMainWindow::connect(this->widget, &ClipWidget::planeMoved, this, &ClipTool::onPlaneMoved);

    auto * settings = this->main_window->getSettings();
    auto pos = settings->value("clip_tool/pos", QPoint(-1, -1)).toPoint();
    if (pos.x() >= 0 && pos.y() >= 0)
        this->widget->move(pos);

    this->widget->setClipPlane(2);
}

void
ClipTool::done()
{
    this->widget->done();
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

    clipBlocks();
}

void
ClipTool::onClose()
{
    for (auto & [id, block] : this->model->getBlocks()) {
        block->setClip(false);
    }
}

void
ClipTool::clipBlocks()
{
    for (auto & [id, block] : this->model->getBlocks()) {
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
    for (auto & [id, block] : this->model->getBlocks()) {
        block->setClipPlane(this->clip_plane);
        block->modified();
        block->update();
    }
}

void
ClipTool::closeEvent(QCloseEvent * event)
{
    auto pos = this->widget->pos();
    auto * settings = this->main_window->getSettings();
    settings->setValue("clip_tool/pos", pos);
}
