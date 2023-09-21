#pragma once

#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget * parent = nullptr);

protected:
    void enterEvent(QEnterEvent * event) override;
    void leaveEvent(QEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * ev) override;

signals:
    void clicked();
};
