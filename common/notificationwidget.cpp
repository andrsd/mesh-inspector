#include "notificationwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QByteArray>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include "clickablelabel.h"

NotificationWidget::NotificationWidget(QWidget * parent) :
    QWidget(parent),
    layout(nullptr),
    text(nullptr),
    dismiss_label(nullptr),
    anim(nullptr)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border-radius: 6px;"
                  "background-color: #307BF6;"
                  "color: #fff;"
                  "font-size: 14px;");
    setUpWidgets();
}

void
NotificationWidget::setUpWidgets()
{
    this->text = new QLabel();

    this->dismiss_label = new ClickableLabel();
    this->dismiss_label->setText("\u2716");
    this->dismiss_label->setStyleSheet("font-weight: bold;");

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(30, 8, 30, 8);
    this->layout->setSpacing(20);
    this->layout->addWidget(this->text);
    this->layout->addWidget(this->dismiss_label);
    setLayout(this->layout);

    connect(this->dismiss_label, &ClickableLabel::clicked, this, &NotificationWidget::onDismiss);
}

void
NotificationWidget::setText(const QString & str)
{
    this->text->setText(str);
}

void
NotificationWidget::onDismiss()
{
    hide();
    emit dismiss();
}

void
NotificationWidget::show(int ms)
{
    QWidget::show();
    QTimer::singleShot(ms, this, &NotificationWidget::onNotificationFadeOut);
}

void
NotificationWidget::onNotificationFadeOut()
{
    auto * effect = new QGraphicsOpacityEffect();
    setGraphicsEffect(effect);

    this->anim = new QPropertyAnimation(effect, QByteArray("opacity"));
    this->anim->setDuration(250);
    this->anim->setStartValue(1);
    this->anim->setEndValue(0);
    connect(this->anim, &QPropertyAnimation::finished, this, &NotificationWidget::hide);
    this->anim->start();
}
