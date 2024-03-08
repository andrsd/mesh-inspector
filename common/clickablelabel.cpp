// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget * parent) : QLabel(parent) {}

void
ClickableLabel::enterEvent(QEnterEvent * event)
{
    auto f = this->font();
    f.setUnderline(true);
    this->setFont(f);
}

void
ClickableLabel::leaveEvent(QEvent * event)
{
    auto f = this->font();
    f.setUnderline(false);
    this->setFont(f);
}

void
ClickableLabel::mouseReleaseEvent(QMouseEvent * ev)
{
    emit clicked();
}
