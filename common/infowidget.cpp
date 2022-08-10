#include "infowidget.h"
#include <QString>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QLabel>

InfoWidget::InfoWidget(QWidget * parent) :
    QWidget(parent),
    opacity(nullptr),
    layout(nullptr),
    text(nullptr)
{
    setAttribute(Qt::WA_StyledBackground, true);
    auto qss = QString("border-radius: 3px;"
                       "background-color: #222;"
                       "color: #fff;"
                       "font-size: 14px;");
    setStyleSheet(qss);
    this->opacity = new QGraphicsOpacityEffect(this);
    this->opacity->setOpacity(0.8);
    setGraphicsEffect(this->opacity);

    this->layout = new QVBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 8);

    this->text = new QLabel();
    this->layout->addWidget(this->text);

    setLayout(this->layout);
}

InfoWidget::~InfoWidget()
{
    delete this->opacity;
    delete this->layout;
    delete this->text;
}

void
InfoWidget::setText(const QString & text)
{
    this->text->setText(text);
}

void
InfoWidget::clear()
{
    this->text->setText("");
}
