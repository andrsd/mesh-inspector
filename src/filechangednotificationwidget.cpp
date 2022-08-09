#include "filechangednotificationwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QString>
#include "common/clickablelabel.h"

FileChangedNotificationWidget::FileChangedNotificationWidget(QWidget * parent) :
    QWidget(parent),
    layout(nullptr),
    text(nullptr),
    reload(nullptr),
    dismiss(nullptr)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("border-radius: 6px;"
                  "background-color: #307BF6;"
                  "color: #fff;"
                  "font-size: 14px;");
    setUpWidgets();
}

FileChangedNotificationWidget::~FileChangedNotificationWidget()
{
    delete this->layout;
    delete this->text;
    delete this->reload;
    delete this->dismiss;
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

    connect(this->reload, SIGNAL(clicked()), this, SLOT(onReload()));
    connect(this->dismiss, SIGNAL(clicked()), this, SLOT(onDismiss()));
}

void
FileChangedNotificationWidget::setFileName(const QString & file_name)
{
    QFileInfo fi(file_name);
    auto text = QString("File '%1' changed").arg(fi.fileName());
    this->text->setText(text);
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
