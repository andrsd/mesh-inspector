// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>

class QVBoxLayout;
class QTextEdit;

class LicenseDialog : public QDialog {
public:
    explicit LicenseDialog(QWidget * parent = nullptr);
    ~LicenseDialog() override;

protected:
    QVBoxLayout * layout;
    QTextEdit * license;
};
