// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common_export.h"
#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget * parent = nullptr);

protected:
    void enterEvent(QEnterEvent * event) override;
    void leaveEvent(QEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * ev) override;

signals:
    void clicked();
};
