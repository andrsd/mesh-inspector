// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>

class QVBoxLayout;
class QLabel;
class ClickableLabel;

class AboutDialog : public QDialog {
public:
    explicit AboutDialog(QWidget * parent = nullptr);
    ~AboutDialog() override;

protected slots:
    void onHomepageClicked();

protected:
    QVBoxLayout * layout;
    QLabel * icon;
    QLabel * title;
    QLabel * version;
    ClickableLabel * homepage;
    QLabel * copyright;
};
