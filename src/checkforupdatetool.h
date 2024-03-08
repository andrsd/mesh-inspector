// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QVersionNumber>

class MainWindow;
class QNetworkAccessManager;
class QNetworkReply;
class QMenu;
class QAction;

class CheckForUpdateTool : public QObject {
public:
    explicit CheckForUpdateTool(MainWindow * main_wnd);
    ~CheckForUpdateTool() override;

    void setupMenu(QMenu * menu);
    void onCheckForUpdate();

public slots:
    void onHttpReply(QNetworkReply * reply);

protected:
    QVersionNumber getVersionFromReply(QNetworkReply * reply);

    MainWindow * main_window;
    QNetworkAccessManager * namgr;
    QAction * check_update_action;
};
