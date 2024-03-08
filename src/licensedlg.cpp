// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "licensedlg.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QFile>
#include <QString>

LicenseDialog::LicenseDialog(QWidget * parent) : QDialog(parent)
{
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setWindowTitle("License");
    setMinimumWidth(500);
    setMinimumHeight(600);

    this->layout = new QVBoxLayout();
    this->layout->addSpacing(8);

    QString text;
    QString file_name(":/license");
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly))
        text = file.readAll();

    this->license = new QTextEdit();
    this->license->setReadOnly(true);
    this->license->setText(text);
    this->layout->addWidget(this->license);

    setLayout(this->layout);
}

LicenseDialog::~LicenseDialog()
{
    delete this->layout;
}
