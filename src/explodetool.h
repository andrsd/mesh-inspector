// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPoint>

class MainWindow;
class Model;
class ExplodeWidget;
class QAction;
class QMenu;

class ExplodeTool : public QObject {
    Q_OBJECT

public:
    explicit ExplodeTool(MainWindow * main_wnd);
    ~ExplodeTool() override;

    void setupWidgets();

public slots:
    void onExplode();
    void onValueChanged(double value);
    void onClose();

protected:
    MainWindow * main_window;
    Model *& model;
    ExplodeWidget * explode;
    QPoint pos;
};
