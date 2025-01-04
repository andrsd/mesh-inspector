// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "explodetool.h"
#include "explodewidget.h"
#include "mainwindow.h"
#include "model.h"
#include <QMenu>
#include <QSettings>
#include "vtkMath.h"
#include "blockobject.h"

ExplodeTool::ExplodeTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    explode(nullptr),
    pos(-1, -1)
{
    auto * settings = this->main_window->getSettings();
    this->pos = settings->value("explode_tool/pos", QPoint(-1, -1)).toPoint();
}

ExplodeTool::~ExplodeTool()
{
    delete this->explode;
}

void
ExplodeTool::setupWidgets()
{
    this->explode = new ExplodeWidget(this->main_window);
    QMainWindow::connect(this->explode,
                         &ExplodeWidget::valueChanged,
                         this,
                         &ExplodeTool::onValueChanged);
    QMainWindow::connect(this->explode, &ExplodeWidget::closed, this, &ExplodeTool::onClose);
    this->explode->setVisible(false);
}

void
ExplodeTool::updateLocation()
{
    if (this->pos.x() == -1 && this->pos.y() == -1) {
        auto width = this->main_window->getRenderWindowWidth();
        int left = (width - this->explode->width()) / 2;
        int top = this->main_window->geometry().height() - this->explode->height() - 10;
        this->pos = QPoint(left, top);
    }
    this->explode->move(this->pos);
}

void
ExplodeTool::onExplode()
{
    this->explode->adjustSize();
    this->explode->show();
    updateLocation();
}

void
ExplodeTool::onValueChanged(double value)
{
    double dist = value / this->explode->range();
    for (auto & it : this->model->getBlocks()) {
        auto block = it.second;
        auto blk_cob = block->getCenterOfBounds();
        vtkVector3d dir;
        vtkMath::Subtract(blk_cob, this->model->getCenterOfBounds(), dir);
        dir.Normalize();
        vtkMath::MultiplyScalar(dir.GetData(), dist);
        block->setPosition(dir[0], dir[1], dir[2]);
    }
}

void
ExplodeTool::onClose()
{
    this->pos = this->explode->pos();
    auto * settings = this->main_window->getSettings();
    settings->setValue("explode_tool/pos", this->pos);
}
