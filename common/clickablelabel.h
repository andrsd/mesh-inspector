#pragma once

#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget * parent = nullptr);

protected:
    virtual void enterEvent(QEnterEvent * event) override;
    virtual void leaveEvent(QEvent * event) override;
    virtual void mouseReleaseEvent(QMouseEvent * ev) override;

signals:
    void clicked();
};
