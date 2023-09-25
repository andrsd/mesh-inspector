#include "explodetool.h"
#include "explodewidget.h"
#include "mainwindow.h"
#include <QMenu>
#include "vtkMath.h"
#include "blockobject.h"

ExplodeTool::ExplodeTool(MainWindow * main_wnd) : main_window(main_wnd), explode(nullptr) {}

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
    this->explode->setVisible(false);
}

void
ExplodeTool::updateLocation()
{
    auto width = this->main_window->getRenderWindowWidth();
    int left = (width - this->explode->width()) / 2;
    int top = this->main_window->geometry().height() - this->explode->height() - 10;
    this->explode->move(left, top);
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
    for (auto & it : this->main_window->getBlocks()) {
        auto * block = it.second;
        auto blk_cob = block->getCenterOfBounds();
        vtkVector3d dir;
        vtkMath::Subtract(blk_cob, this->main_window->getCenterOfBounds(), dir);
        dir.Normalize();
        vtkMath::MultiplyScalar(dir.GetData(), dist);
        block->setPosition(dir[0], dir[1], dir[2]);
    }
}
