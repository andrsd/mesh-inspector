// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "aboutdlg.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QDesktopServices>
#include "common/clickablelabel.h"
#include "meshinspectorconfig.h"

namespace {

QString HOME_PAGE_URL("https://github.com/andrsd/mesh-inspector");

}

AboutDialog::AboutDialog(QWidget * parent) :
    QDialog(parent),
    layout(nullptr),
    icon(nullptr),
    title(nullptr),
    version(nullptr),
    homepage(nullptr),
    copyright(nullptr)
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

    this->version = new QLabel(QString("Version %1").arg(MESH_INSPECTOR_VERSION));
    font = this->version->font();
    font.setPointSize(0.9 * font.pointSize());
    this->version->setFont(font);
    this->version->setAlignment(Qt::AlignHCenter);
    this->layout->addWidget(this->version);

    this->homepage = new ClickableLabel();
    this->homepage->setText(HOME_PAGE_URL);
    auto plt = QApplication::palette();
    auto link_clr = plt.color(QPalette::Link);
    this->homepage->setStyleSheet(QString("color: %1").arg(link_clr.name()));
    font = this->homepage->font();
    font.setPointSize(0.9 * font.pointSize());
    this->homepage->setFont(font);
    this->homepage->setAlignment(Qt::AlignHCenter);
    this->layout->addWidget(this->homepage);

    this->layout->addSpacing(20);

    this->copyright = new QLabel(QString("%1").arg(MESH_INSPECTOR_COPYRIGHT));
    font = this->copyright->font();
    font.setPointSize(0.9 * font.pointSize());
    this->copyright->setFont(font);
    this->copyright->setAlignment(Qt::AlignHCenter);
    this->layout->addWidget(this->copyright);

    setLayout(this->layout);

    connect(this->homepage, &ClickableLabel::clicked, this, &AboutDialog::onHomepageClicked);
}

AboutDialog::~AboutDialog()
{
    delete this->layout;
    delete this->icon;
    delete this->title;
    delete this->version;
    delete this->copyright;
}

void
AboutDialog::onHomepageClicked()
{
    QDesktopServices::openUrl(QUrl(HOME_PAGE_URL));
}
