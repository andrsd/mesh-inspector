// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MainWindow;
class Model;

class ClipTool : public QObject {
    Q_OBJECT

public:
    explicit ClipTool(MainWindow * main_wnd);
    ~ClipTool() override;

    void setupWidgets();
    void updateLocation();

public slots:
    void onClip();

protected:
    MainWindow * main_window;
    Model *& model;
};
