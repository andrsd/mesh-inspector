// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class ClickableLabel;

class ClipWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipWidget(QWidget * parent = nullptr);

signals:
    void closed();

protected slots:
    void onClose();

protected:
    QGraphicsOpacityEffect * opacity;
    QHBoxLayout * layout;
    ClickableLabel * close;
};
