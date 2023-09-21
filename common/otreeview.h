#pragma once

#include <QTreeView>
#include <QString>

class QPaintEvent;
class QMouseEvent;

class OTreeView : public QTreeView {
public:
    explicit OTreeView(QWidget * parent = nullptr);

    virtual void setEmptyMessage(const QString & msg);

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;

    QString empty_message;
};
