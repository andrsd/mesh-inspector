#include "aboutdlg.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QIcon>
#include "meshinspectorconfig.h"

AboutDialog::AboutDialog(QWidget * parent) :
    layout(nullptr),
    icon(nullptr),
    title(nullptr),
    text(nullptr)
{
    setWindowFlag(Qt::CustomizeWindowHint, true);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);

    this->layout = new QVBoxLayout();
    this->layout->addSpacing(8);

    QIcon ico = QApplication::windowIcon();
    this->icon = new QLabel();
    this->icon->setPixmap(ico.pixmap(64, 64));
    this->layout->addWidget(this->icon, 0, Qt::AlignHCenter);

    this->title = new QLabel(MESH_INSPECTOR_APP_NAME);
    QFont font = this->title->font();
    font.setBold(true);
    font.setPointSize(int(1.2 * font.pointSize()));
    this->title->setFont(font);
    this->title->setAlignment(Qt::AlignHCenter);
    this->layout->addWidget(this->title);

    QString t =
        QString("Version %1\n\n%2").arg(MESH_INSPECTOR_VERSION).arg(MESH_INSPECTOR_COPYRIGHT);
    this->text = new QLabel(t);
    font = this->text->font();
    font.setPointSize(0.9 * font.pointSize());
    this->text->setFont(font);
    this->text->setAlignment(Qt::AlignHCenter);
    this->layout->addWidget(this->text);

    setLayout(this->layout);
}

AboutDialog::~AboutDialog()
{
    delete this->layout;
    delete this->icon;
    delete this->title;
    delete this->text;
}
