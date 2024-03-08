// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "otreeview.h"
#include <QPainter>
#include <QPaintEvent>

OTreeView::OTreeView(QWidget * parent) : QTreeView(parent), empty_message("No items") {}

void
OTreeView::setEmptyMessage(const QString & msg)
{
    this->empty_message = msg;
}

void
OTreeView::paintEvent(QPaintEvent * event)
{
    QTreeView::paintEvent(event);

    if (model() && model()->rowCount(rootIndex()) > 0)
        return;

    QFont fnt = font();
    fnt.setPointSizeF(fnt.pointSizeF() * 0.9);
    // The view is empty
    auto painter = new QPainter(viewport());
    painter->setFont(fnt);
    auto rc = rect();
    rc.adjust(0, 10, 0, 0);
    painter->drawText(rc, Qt::AlignHCenter | Qt::AlignTop, this->empty_message);
    delete painter;
}

void
OTreeView::mousePressEvent(QMouseEvent * event)
{
    auto item = indexAt(event->pos());
    if (item.isValid())
        QTreeView::mousePressEvent(event);
    else
        this->clearSelection();
}
