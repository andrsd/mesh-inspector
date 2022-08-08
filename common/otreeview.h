#pragma once

#include <QTreeView>
#include <QString>

class QPaintEvent;
class QMouseEvent;

class OTreeView : public QTreeView {
public:
    OTreeView(QWidget * parent = nullptr);

    virtual void setEmptyMessage(const QString & msg);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);

    QString empty_message;
};
