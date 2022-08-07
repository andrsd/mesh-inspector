#include "expandablewidget.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

ExpandableWidget::ExpandableWidget(const QString & text, QWidget * parent) :
    QWidget(parent),
    caption(text),
    widget(nullptr),
    expand_button(nullptr),
    label(nullptr),
    layout(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->expand_button = new QPushButton();
    this->expand_button->setCheckable(true);
    this->expand_button->setChecked(false);
    this->expand_button->setFlat(true);
    this->expand_button->setFixedSize(20, 20);
    this->expand_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->expand_button->setStyleSheet("QPushButton {"
                                       "  border: none;"
                                       "  text-align: left;"
                                       "}"
                                       "QPushButton:checked {"
                                       "  border: none;"
                                       "  text-align: left;"
                                       "}");

    this->label = new QLabel();
    this->label->setText(this->caption);
    this->label->setStyleSheet("color: #444;"
                               "font-weight: bold;");
    this->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->layout = new QGridLayout();
    this->layout->setSpacing(6);
    this->layout->setHorizontalSpacing(0);
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->addWidget(this->expand_button, 0, 0);
    this->layout->addWidget(this->label, 0, 1);
    setLayout(this->layout);

    connect(this->expand_button, SIGNAL(toggled()), this, SLOT(onExpandToggled()));
}

ExpandableWidget::~ExpandableWidget()
{
    delete this->widget;
    delete this->expand_button;
    delete this->label;
    delete this->layout;
}

void
ExpandableWidget::setLabel(const QString & text)
{
    this->label->setText(text);
}

void
ExpandableWidget::setWidget(QWidget * widget)
{
    this->widget = widget;
    this->layout->addWidget(this->widget, 1, 0, 1, 2);

    bool checked = this->expand_button->isChecked();
    this->widget->setVisible(checked);
    setExpandButtonText(checked);
}

void
ExpandableWidget::onExpandToggled(bool checked)
{
    this->widget->setVisible(checked);
    setExpandButtonText(checked);
}

void
ExpandableWidget::setExpandButtonText(bool checked)
{
    if (checked)
        this->expand_button->setText("\u25BC");
    else
        this->expand_button->setText("\u25B6");
}

void
ExpandableWidget::setNumberOfItems(int num)
{
    if (num == 0)
        this->label->setText(this->caption);
    else
        this->label->setText(QString("%1 (%2)").arg(this->caption).arg(num));
}
