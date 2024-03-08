// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <map>
#include <QString>
#include <QColor>

class ColorProfile {
public:
    ColorProfile(const QString & name, const std::map<QString, QColor> & color_map);

    const QString & getName() const;
    const QColor & getColor(const QString & name) const;

protected:
    QString name;
    std::map<QString, QColor> colors;

    QColor black;
};
