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
ColorProfile::getColor(const QString & name) const
{
    const auto & it = this->colors.find(name);
    if (it != this->colors.end())
        return it->second;
    else
        return this->black;
}
