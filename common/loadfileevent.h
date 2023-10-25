#pragma once

#include "common_export.h"
#include <QEvent>
#include <QString>

class LoadFileEvent : public QEvent {
public:
    explicit LoadFileEvent(const QString & file_name);

    const QString & fileName() const;

protected:
    QString file_name;

public:
    static const QEvent::Type type;
};
