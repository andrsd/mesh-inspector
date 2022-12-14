#pragma once

#include <QWidget>

class QHBoxLayout;
class QLabel;
class ClickableLabel;
class QPropertyAnimation;

class NotificationWidget : public QWidget {
    Q_OBJECT

public:
    NotificationWidget(QWidget * parent = nullptr);

    void setText(const QString & text);
    void show(int ms = 2000);

signals:
    void dismiss();

public slots:
    void onDismiss();
    void onNotificationFadeOut();

protected:
    void setUpWidgets();

    QHBoxLayout * layout;
    QLabel * text;
    ClickableLabel * dismiss_label;
    QPropertyAnimation * anim;
};
