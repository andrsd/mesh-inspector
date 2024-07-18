// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cliptool.h"
#include "mainwindow.h"
#include "model.h"

ClipTool::ClipTool(MainWindow * main_wnd) : main_window(main_wnd), model(main_wnd->getModel()) {}

ClipTool::~ClipTool() {}

void
ClipTool::setupWidgets()
{
}

void
ClipTool::updateLocation()
{
}

void
ClipTool::onClip()
{
}
