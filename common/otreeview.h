#pragma once

#include "common_export.h"
#include <QTreeView>
#include <QString>

class QPaintEvent;
class QMouseEvent;

class COMMON_EXPORT OTreeView : public QTreeView {
public:
    explicit OTreeView(QWidget * parent = nullptr);

    virtual void setEmptyMessage(const QString & msg);

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;

    QString empty_message;
};
