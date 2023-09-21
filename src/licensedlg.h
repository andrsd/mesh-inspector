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
