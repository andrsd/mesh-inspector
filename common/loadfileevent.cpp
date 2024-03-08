// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "loadfileevent.h"

const QEvent::Type LoadFileEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

LoadFileEvent::LoadFileEvent(const QString & file_name) : QEvent(type), file_name(file_name) {}

const QString &
LoadFileEvent::fileName() const
{
    return this->file_name;
}
