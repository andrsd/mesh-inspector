// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "horzline.h"

HorzLine::HorzLine(QWidget * parent) : QFrame(parent)
{
    this->setFrameShape(QFrame::HLine);
    this->setFrameShadow(QFrame::Plain);
    this->setContentsMargins(0, 10, 0, 10);
    this->setFixedHeight(10);
    this->setStyleSheet("color: #D3D3D3;");
}
