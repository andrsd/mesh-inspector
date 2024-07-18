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

ClipTool::ClipTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    widget(nullptr),
    clip_plane(vtkPlane::New())
{
    setPlaneNormal(QVector3D(0, 0, -1));
}

ClipTool::~ClipTool()
{
    delete this->widget;
    this->clip_plane->Delete();
}

void
ClipTool::setPlaneNormal(const QVector3D & normal)
{
    this->clip_plane->SetNormal(normal.x(), normal.y(), normal.z());
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
