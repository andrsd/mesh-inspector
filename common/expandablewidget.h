// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common_export.h"
#include <QWidget>
#include <QString>

class QPushButton;
class QLabel;
class QGridLayout;

class ExpandableWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExpandableWidget(const QString & text, QWidget * parent = nullptr);
    ~ExpandableWidget() override;

    void setLabel(const QString & text);
    void setNumberOfItems(int num);
    void setWidget(QWidget * w);
    void setExpanded(bool expanded);

protected slots:
    void onExpandToggled(bool checked);

protected:
    void setExpandButtonText(bool checked);

    QString caption;
    QWidget * widget;
    QPushButton * expand_button;
    QLabel * label;
    QGridLayout * layout;
};
