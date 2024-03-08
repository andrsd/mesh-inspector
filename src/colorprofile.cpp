// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "colorprofile.h"

ColorProfile::ColorProfile(const QString & name, const std::map<QString, QColor> & color_map) :
    name(name),
    colors(color_map),
    black(0, 0, 0)
{
}

const QString &
ColorProfile::getName() const
{
    return this->name;
}

const QColor &
ColorProfile::getColor(const QString & clr_name) const
{
    const auto & it = this->colors.find(clr_name);
    if (it != this->colors.end())
        return it->second;
    else
        return this->black;
}
