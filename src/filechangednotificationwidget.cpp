// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "filechangednotificationwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QString>
#include <QShortcut>
#include "common/clickablelabel.h"

FileChangedNotificationWidget::FileChangedNotificationWidget(QWidget * parent) :
    QWidget(parent),
    layout(nullptr),
    text(nullptr),
    reload(nullptr),
    dismiss(nullptr),
    dismiss_shortcut(nullptr),
    reload_shortcut(nullptr)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border-radius: 6px;"
                  "background-color: #307BF6;"
                  "color: #fff;"
                  "font-size: 14px;");
    setUpWidgets();

    this->dismiss_shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), parent);
    this->dismiss_shortcut->setEnabled(false);
    connect(this->dismiss_shortcut,
            &QShortcut::activated,
            this,
            &FileChangedNotificationWidget::onDismiss);

    this->reload_shortcut = new QShortcut(QKeySequence("Ctrl+R"), parent);
    this->reload_shortcut->setEnabled(false);
    connect(this->reload_shortcut,
            &QShortcut::activated,
            this,
            &FileChangedNotificationWidget::onReload);
}

FileChangedNotificationWidget::~FileChangedNotificationWidget()
{
    delete this->layout;
    delete this->text;
    delete this->reload;
    delete this->dismiss;
    delete this->dismiss_shortcut;
    delete this->reload_shortcut;
}

void
FileChangedNotificationWidget::setUpWidgets()
{
    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(30, 8, 30, 8);
    this->layout->setSpacing(20);

    this->text = new QLabel();
    this->layout->addWidget(this->text);

    this->reload = new ClickableLabel();
    this->reload->setText("Reload");
    this->reload->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->reload);

    this->dismiss = new ClickableLabel();
    this->dismiss->setText("Dismiss");
    this->dismiss->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->dismiss);

    setLayout(this->layout);

    connect(this->reload, &ClickableLabel::clicked, this, &FileChangedNotificationWidget::onReload);
    connect(this->dismiss,
            &ClickableLabel::clicked,
            this,
            &FileChangedNotificationWidget::onDismiss);
}

void
FileChangedNotificationWidget::setFileName(const QString & file_name)
{
    QFileInfo fi(file_name);
    auto str = QString("File '%1' changed").arg(fi.fileName());
    this->text->setText(str);
}

void
FileChangedNotificationWidget::onReload()
{
    hide();
    emit reloaded();
}

void
FileChangedNotificationWidget::onDismiss()
{
    hide();
    emit dismissed();
}

void
FileChangedNotificationWidget::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);

    this->dismiss_shortcut->setEnabled(true);
    this->reload_shortcut->setEnabled(true);
}

void
FileChangedNotificationWidget::hideEvent(QHideEvent * event)
{
    QWidget::hideEvent(event);

    this->dismiss_shortcut->setEnabled(false);
    this->reload_shortcut->setEnabled(false);
}
