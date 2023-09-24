#pragma once

#include "common_export.h"
#include <QEvent>
#include <QString>

class COMMON_EXPORT LoadFileEvent : public QEvent {
public:
    explicit LoadFileEvent(const QString & file_name);

    const QString & fileName() const;

protected:
    QString file_name;

public:
    static const QEvent::Type type;
};
